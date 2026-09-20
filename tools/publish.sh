#!/usr/bin/env bash
# Cria o repositorio no GitHub (se nao existir), envia o codigo e dispara a compilacao.
# Uso (Termux):
#   read -rs GH_TOKEN; export GH_TOKEN      # cola o token e aperta Enter (nao aparece na tela)
#   bash tools/publish.sh
# Opcoes (variaveis de ambiente): GH_USER, REPO, VISIBILITY=public|private, SUBVERSION, PLATFORM
set -euo pipefail

GH_USER="${GH_USER:-in3zalo-gk}"
REPO="${REPO:-NewbX-DarkManager}"
VISIBILITY="${VISIBILITY:-public}"
SUBVERSION="${SUBVERSION:-1}"
PLATFORM="${PLATFORM:-Android}"
: "${GH_TOKEN:?defina GH_TOKEN primeiro: read -rs GH_TOKEN; export GH_TOKEN}"

command -v git >/dev/null  || { echo "instale: pkg install git"; exit 1; }
command -v curl >/dev/null || { echo "instale: pkg install curl"; exit 1; }

api() { curl -fsS -H "Authorization: Bearer $GH_TOKEN" -H "Accept: application/vnd.github+json" "$@"; }

login=$(api https://api.github.com/user | grep -o '"login": *"[^"]*"' | head -1 | sed 's/.*: *"\(.*\)"/\1/')
if [ "$login" != "$GH_USER" ]; then
  echo "O token pertence a '$login', mas GH_USER='$GH_USER'. Ajuste GH_USER ou use o token certo."; exit 1
fi
echo "login ok: $login"

if api "https://api.github.com/repos/$GH_USER/$REPO" >/dev/null 2>&1; then
  echo "repo $GH_USER/$REPO ja existe, vou so atualizar"
else
  priv=false; [ "$VISIBILITY" = "private" ] && priv=true
  api -X POST https://api.github.com/user/repos \
    -d "{\"name\":\"$REPO\",\"private\":$priv,\"description\":\"Newb X Dark Manager - dark fantasy RenderDragon shader (base: Newb X Legacy)\"}" >/dev/null
  echo "repo criado: $GH_USER/$REPO ($VISIBILITY)"
fi

cd "$(dirname "$0")/.."
[ -d .git ] || git init -q -b main
git add -A
git -c user.name="${GIT_NAME:-$GH_USER}" -c user.email="${GIT_EMAIL:-$GH_USER@users.noreply.github.com}" \
    commit -q -m "Newb X Dark Manager" 2>/dev/null || echo "(nada novo para commitar)"
git branch -M main
# token usado so nesta chamada; nao fica salvo no .git/config
git push "https://x-access-token:${GH_TOKEN}@github.com/$GH_USER/$REPO.git" HEAD:main
git remote get-url origin >/dev/null 2>&1 || git remote add origin "https://github.com/$GH_USER/$REPO.git"

sleep 3
if api -X POST "https://api.github.com/repos/$GH_USER/$REPO/actions/workflows/build.yml/dispatches" \
     -d "{\"ref\":\"main\",\"inputs\":{\"subversion\":\"$SUBVERSION\",\"platform\":\"$PLATFORM\"}}"; then
  echo "build disparado."
else
  echo "Nao consegui disparar automaticamente (token sem permissao Actions?). Rode manualmente em Actions > build."
fi
echo "Acompanhe: https://github.com/$GH_USER/$REPO/actions"
