'use strict'
// Antora 3.1 cannot open a local content source whose .git entry is a file
// (a gitdir reference), which is the case in every linked git worktree.  This
// extension detects that situation in the playbookBuilt event — before content
// aggregation — and rewrites the content source url to the main worktree (which
// has a proper .git directory).  It also resolves any literal "HEAD" branch
// pattern to the branch actually checked out in the linked worktree so the
// correct ref is built.

const fs = require('fs')
const path = require('path')

module.exports.register = function () {
  this.once('playbookBuilt', ({ playbook }) => {
    const playbookDir = playbook.dir
    const dotGit = path.join(playbookDir, '.git')

    let stat
    try {
      stat = fs.statSync(dotGit)
    } catch {
      return // no .git at all
    }

    if (stat.isDirectory()) return // ordinary checkout, nothing to fix

    // .git is a file → we are inside a linked git worktree.
    let gitfileContent
    try {
      gitfileContent = fs.readFileSync(dotGit, 'utf8').trim()
    } catch {
      return
    }

    // Format: "gitdir: /absolute/path/to/.git/worktrees/<name>"
    const m = gitfileContent.match(/^gitdir:\s*(.+)$/)
    if (!m) return

    const worktreeGitdir = path.resolve(playbookDir, m[1].trim())

    // Locate the main .git directory via the commondir file.
    let mainGitdir
    try {
      const rel = fs.readFileSync(path.join(worktreeGitdir, 'commondir'), 'utf8').trim()
      mainGitdir = path.resolve(worktreeGitdir, rel)
    } catch {
      // Fallback: strip trailing /worktrees/<name>.
      mainGitdir = worktreeGitdir.replace(/[/\\]worktrees[/\\][^/\\]+$/, '')
    }

    const mainRepoRoot = path.dirname(mainGitdir)

    // Resolve the branch checked out in this worktree.
    let currentBranch
    try {
      const head = fs.readFileSync(path.join(worktreeGitdir, 'HEAD'), 'utf8').trim()
      const ref = head.match(/^ref:\s*refs\/heads\/(.+)$/)
      if (ref) currentBranch = ref[1]
    } catch {
      // Detached HEAD — leave branch patterns unchanged.
    }

    // Patch every local content source whose url resolves to the worktree root.
    for (const source of playbook.content.sources) {
      if (!source.url) continue
      if (/^https?:\/\/|^git@/.test(source.url)) continue
      if (path.resolve(playbookDir, source.url) !== playbookDir) continue

      source.url = mainRepoRoot

      if (currentBranch && Array.isArray(source.branches)) {
        source.branches = source.branches.map((b) => (b === 'HEAD' ? currentBranch : b))
      }
    }
  })
}
