'use strict'
// Antora 3.1 cannot open a local content source whose .git entry is a file
// (a gitdir reference), which is the case in every linked git worktree.  This
// extension detects that situation in the playbookBuilt event — before content
// aggregation — and rewrites the content source url to the canonical git
// repository root so Antora can open it.  For a normal repo that is the parent
// of the .git directory; for a bare repo (worktrees checked out alongside
// optional.git) it is the bare repo directory itself.  It also resolves any
// literal "HEAD" branch pattern to the branch actually checked out in the
// linked worktree so the correct ref is built.
//
// Bare-repo extra: Antora/isomorphic-git reads refs/remotes/<remote>/<branch>
// rather than refs/heads/<branch> when the content source is a bare repo.
// If the user fetches from a secondary remote (e.g. bbgithub) the local head
// moves forward but origin's tracking ref stays behind, causing Antora to read
// stale content.  We therefore sync every remote-tracking ref for the current
// branch to the local head before Antora aggregates content.

const fs = require('fs')
const path = require('path')

// Read a git ref SHA, following symrefs and checking loose refs then packed-refs.
function readRef (gitdir, refName) {
  try {
    const content = fs.readFileSync(path.join(gitdir, refName), 'utf8').trim()
    if (content.startsWith('ref: ')) return readRef(gitdir, content.slice(5))
    return content
  } catch {}
  try {
    const packed = fs.readFileSync(path.join(gitdir, 'packed-refs'), 'utf8')
    for (const line of packed.split('\n')) {
      if (line.startsWith('#') || !line.trim()) continue
      const [sha, ref] = line.trim().split(' ')
      if (ref === refName) return sha
    }
  } catch {}
  return null
}

// Write a loose git ref, creating parent directories as needed.
function writeRef (gitdir, refName, sha) {
  try {
    const refPath = path.join(gitdir, refName)
    fs.mkdirSync(path.dirname(refPath), { recursive: true })
    fs.writeFileSync(refPath, sha + '\n', 'utf8')
  } catch {}
}

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

    // For a normal repo mainGitdir = <root>/.git, so the working tree is its parent.
    // For a bare repo mainGitdir IS the repository (no separate working tree), so
    // path.dirname() would land in the bare repo's parent — a plain directory that
    // Antora can't open.  Detect the bare case by reading core.bare from the config.
    let mainRepoRoot = path.dirname(mainGitdir)
    let isBare = false
    try {
      const config = fs.readFileSync(path.join(mainGitdir, 'config'), 'utf8')
      const worktreeMatch = config.match(/^\s*worktree\s*=\s*(.+?)\s*$/im)
      const bareMatch = config.match(/^\s*bare\s*=\s*true\s*$/im)
      if (worktreeMatch) {
        mainRepoRoot = path.resolve(mainGitdir, worktreeMatch[1].trim())
      } else if (bareMatch) {
        mainRepoRoot = mainGitdir
        isBare = true
      }
    } catch {
      // keep default (path.dirname)
    }

    // Resolve the branch checked out in this worktree.
    let currentBranch
    try {
      const head = fs.readFileSync(path.join(worktreeGitdir, 'HEAD'), 'utf8').trim()
      const ref = head.match(/^ref:\s*refs\/heads\/(.+)$/)
      if (ref) currentBranch = ref[1]
    } catch {
      // Detached HEAD — leave branch patterns unchanged.
    }

    // In a bare repo Antora/isomorphic-git reads refs/remotes/<remote>/<branch>
    // rather than refs/heads/<branch>.  We therefore:
    //   1. Always create/update refs/remotes/origin/<branch> from the local head.
    //      This guarantees Antora can find the branch even when it has never been
    //      pushed anywhere (new feature branch), is only on a fork remote, or
    //      origin's tracking ref is simply stale.
    //   2. Sync any other existing remote-tracking refs for the branch so they
    //      don't produce a duplicate stale version.
    if (isBare && currentBranch) {
      const localSHA = readRef(mainGitdir, `refs/heads/${currentBranch}`)
      if (localSHA) {
        const originRef = `refs/remotes/origin/${currentBranch}`
        if (readRef(mainGitdir, originRef) !== localSHA) {
          writeRef(mainGitdir, originRef, localSHA)
        }
        try {
          for (const remote of fs.readdirSync(path.join(mainGitdir, 'refs', 'remotes'))) {
            if (remote === 'origin') continue
            const remoteRef = `refs/remotes/${remote}/${currentBranch}`
            const sha = readRef(mainGitdir, remoteRef)
            if (sha && sha !== localSHA) writeRef(mainGitdir, remoteRef, localSHA)
          }
        } catch {}
      }
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
