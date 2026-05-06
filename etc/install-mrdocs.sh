#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# install-mrdocs.sh — Download and install MrDocs from GitHub releases.
#
# Asset name is MrDocs-{version}-{OS}.tar.gz; the version is embedded in the
# filename so "latest" is resolved by following the releases/latest redirect
# rather than calling the GitHub API (which may be blocked in some networks).

set -euo pipefail

usage() {
    cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Download and install MrDocs from GitHub releases into a local directory.

Options:
  --version VERSION    Release tag to install, e.g. v0.8.0  (default: latest)
  --install-dir DIR    Installation prefix                   (default: .tools/mrdocs)
  --os OS              Target OS: Linux or Darwin            (default: auto-detect)
  -h, --help           Show this help message and exit

When VERSION is 'latest', the current release is resolved by following the
GitHub releases/latest redirect — no GitHub API call is required.

Examples:
  $(basename "$0")
  $(basename "$0") --version v0.8.0
  $(basename "$0") --version latest --install-dir /opt/mrdocs --os Linux
EOF
}

version=latest
install_dir=.tools/mrdocs
os=$(uname -s)

while [[ $# -gt 0 ]]; do
    case $1 in
        --version)    version=$2;     shift 2 ;;
        --install-dir) install_dir=$2; shift 2 ;;
        --os)         os=$2;          shift 2 ;;
        -h|--help)    usage; exit 0 ;;
        *) printf 'Unknown option: %s\n\n' "$1" >&2; usage >&2; exit 1 ;;
    esac
done

if [[ "$version" == "latest" ]]; then
    tag=$(curl -fsSLI -o /dev/null -w '%{url_effective}' \
        https://github.com/cppalliance/mrdocs/releases/latest \
        | grep -o 'v[0-9][^/]*')
    if [[ -z "$tag" ]]; then
        printf 'error: failed to resolve latest MrDocs version\n' >&2
        exit 1
    fi
else
    tag=$version
fi

ver=${tag#v}
tarball="MrDocs-${ver}-${os}.tar.gz"
url="https://github.com/cppalliance/mrdocs/releases/download/${tag}/${tarball}"

mkdir -p "$install_dir"
printf 'Downloading MrDocs %s from %s\n' "$tag" "$url" >&2
curl -fsSL "$url" | tar xz -C "$install_dir" --strip-components=1
printf 'MrDocs %s installed to %s\n' "$tag" "$install_dir" >&2
