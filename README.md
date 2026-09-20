# Newb X Dark Manager

Variante **Dark Fantasy** do Newb X Legacy (shader RenderDragon baseado em `material.bin`).
Feita para Minecraft Bedrock **1.26.50+**, Android com **MB Loader / MB Loader 2**, com foco em FPS em aparelhos de entrada.

> **Status honesto:** o código-fonte foi modificado e passou em verificações estáticas, mas **os `material.bin` ainda não foram
> compilados** (a compilação exige `lazurite` + `shaderc` baixados da internet). Eles saem do build no GitHub Actions
> (ou local) — veja "Como compilar". Nada aqui foi testado dentro do jogo.

## Compatibilidade real com 26.50

O formato dos materiais mudou no 1.26.50. Aplicado igual ao upstream (`devendrn/newb-x-mcbe`, PR #108):

- `requirements.txt`: `lazurite==0.10.0` (era 0.9.0)
- `tool/util.py`: fontes de materiais `src-materials-1.26.51.zip` (era 1.26.40)
- `min_supported_mc_version = [1, 26, 50]` em `pack_config.toml`
- `SunMoon/vertex.sc`: removida a linha `mat4 model = u_model[0];`

O `.material.bin` no formato certo só é gerado quando o `build.sh setup` baixa essas fontes e o `shaderc`.

## Como compilar

**GitHub Actions (recomendado, funciona só com o celular):**

```bash
read -rs GH_TOKEN; export GH_TOKEN     # cola o token (não aparece na tela)
bash tools/publish.sh                  # cria o repo, envia o código e dispara o build
```

Depois: Actions > build > artifact `Newb-X-Dark-Manager-android` (o `.mcpack`).
Token: classic com escopos `repo` + `workflow`, ou fine-grained com Administration, Contents, Workflows e Actions em leitura/escrita.

**Local (PC/Linux):** `pip install -r requirements.txt && ./build.sh setup && ./build.sh pack -p android -v 1`
(saída em `build/pack-android`, `build/*.mcpack`; use `-p multiplatform` para o outro perfil).

Verificação sem internet: `python3 tools/check_static.py` (includes, macros NL_ indefinidas em todas as variantes de subpack).
Verificação pós-build: `python3 tools/validate_pack.py --profile android` (13 `material.bin` em cada pasta, manifest, estrutura do mcpack).

## Configuração (`src/newb/config.h`, bloco "main tuning knobs")

| Pedido | Macro | Padrão |
|---|---|---|
| FOG_DENSITY | `NL_FOG_DENSITY` | 1.0 |
| FOG_DISTANCE | `NL_FOG_DISTANCE` | 0.72 |
| NIGHT_DARKNESS | `NL_NIGHT_DARKNESS` | 0.55 |
| SUN_INTENSITY | `NL_SUNLIGHT_INTENSITY` | 3.0 |
| MOON_INTENSITY | `NL_MOON_INTENSITY` | 1.0 |
| AMBIENT_LIGHT | `NL_AMBIENT_LIGHT` | 0.85 |
| TORCH_LIGHT | `NL_TORCHLIGHT_INTENSITY` | 1.2 |
| NETHER_FOG | `NL_NETHER_FOG` (+ `NL_NETHER_FOG_BRIGHTNESS`) | 1.5 |
| END_FOG | `NL_END_FOG` (+ `NL_END_FOG_START`) | 1.3 |
| UNDERWATER_FOG | `NL_UNDERWATER_FOG` | 1.1 |
| COLOR_SATURATION | `NL_SATURATION` | 0.86 |
| CONTRAST | `NL_CONTRAST` | 1.12 |
| BLOOM_STRENGTH | `NL_BLOOM_STRENGTH` | 0.10 |

Mantive o padrão `NL_*` do projeto em vez de criar outro sistema.

## O que foi implementado

- **Fog:** `functions/fog.h`. Ambiente detectado por `FOG_COLOR`/`FOG_CONTROL` (mesmo método do Newb): Overworld (densidade + distância), Nether, End (curva própria) e debaixo d'água separados. Roda só em vertex shader.
- **Noite** bem mais escura que a vanilla (ambiente do céu + brilho mínimo), lua fria; **contraste** e **saturação** menores, sombras frias e luzes quentes; **tochas** quentes e com leve tremulação.
- **Céu:** azul mais profundo, pôr/nascer do sol laranja-avermelhado, noite azul-marinho, End roxo sombrio, Nether ambiente vermelho escuro.
- **Água** continua leve (sem SSR nem passes extras); tom mais frio e fog subaquática própria.
- **Subpack "Dark Fantasy — Low"** (`DARK_LOW`): desliga pseudo-bloom, god rays, ondas/reflexos extras de água, extras de nuvem/aurora/estrela cadente/arco-íris, névoa ondulada, animações e brilho pulsante. Mantém fog (inclusive Nether/End/água), cores, iluminação, céu e atmosfera.

## Limitações (ditas com exatidão)

- **Fog por bioma no Overworld: NÃO implementado.** O shader não recebe o bioma; só `FOG_COLOR`/`FOG_CONTROL`. Trocar distâncias por bioma via `client_biome.json` quebraria a detecção de chuva/Nether/água do Newb, e não dá para testar aqui. Por isso: fog única no Overworld (com variação por horário e chuva), e Nether/End/água distintos. Os biomas do Nether mantêm a cor de fog vanilla de cada um (a expectativa é diferenciar Crimson/Warped/Soul Sand/Basalt, mas **não confirmado no jogo**).
- **Bloom real não existe** (materiais não têm passe de pós-processo). `NL_BLOOM_STRENGTH` é só um realce barato de altas luzes.
- `BiomeID` existe nas uniforms do projeto, mas não é usado: não há garantia de que o motor preencha isso.
- `NL_EXTRA_PLANTS_WAVE` (desligado por padrão) não recebeu o update de atlas do upstream.
- Valores de cor/fog são um ponto de partida: precisam ser ajustados no jogo.

## Créditos e licença

Baseado em Newb X Legacy / Newb Shader (devendrn e colaboradores), código-fonte sob licença MIT.
Autores no manifest: edite `authors` em `src/newb/pack_config.toml`. Ícone: `assets/pack_icon.png` (ainda o do Newb; troque se quiser).
