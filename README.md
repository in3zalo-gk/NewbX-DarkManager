# Newb X Dark Manager

Shader **Dark Fantasy** para Minecraft Bedrock (RenderDragon), baseado no código MIT do Newb X Legacy.
Alvo: **Minecraft 1.26.50+**, Android de entrada/intermediário (tipo Moto G24), prioridade em FPS.

## Como funciona (resumo)

O Minecraft Bedrock desenha tudo com "materiais" (`material.bin`). Este projeto substitui esses materiais por versões
próprias, escritas em GLSL e compiladas com `lazurite` + `shaderc`. **Não é um pack de texturas** e não usa Vibrant Visuals:
é um shader de material, leve, sem passes extras de pós-processamento.

- Como o shader não recebe "qual é o bioma", ele deduz o ambiente (Overworld, Nether, End, debaixo d'água, chuva) a partir
  de `FOG_COLOR` e `FOG_CONTROL`, que o próprio jogo fornece.
- **Fog por bioma:** cada bioma do Overworld aponta (em `biomes_client.json`) para uma configuração de fog em `fogs/`.
  A distância inicial da névoa vira `FOG_CONTROL` (mais denso = começa mais perto) e a cor da fog vira `FOG_COLOR`,
  cujo tom tinge céu/horizonte/ambiente (`NL_BIOME_TINT`).
- A iluminação é calculada por vértice (barata): sol/lua, ambiente do céu, tochas, sombras, luz nas folhas, poças de chuva.
- Nether e End são detectados e têm fog, ambiente e céu próprios.

## O que precisa

- Minecraft Bedrock **1.26.50 ou superior**.
- Um loader de materiais: **MB Loader / MB Loader 2** (Android) ou **MaterialBinLoader/Patched Minecraft**. Sem loader o shader não é carregado.
- Compilar o pack (o GitHub Actions faz isso; veja "Como compilar"). O jogo só lê `material.bin`, não o código-fonte.

## Modos na engrenagem (subpacks)

| Opção | Para que serve |
|---|---|
| **Dark Fantasy — Completo** (padrão, recomendado) | Tudo ligado: fog por bioma, chuva com poças, luz nas folhas, aurora, bloom leve |
| **Dark Fantasy — Low** | Mais FPS: sem bloom, raios, luz nas folhas, ondas, aurora e brilho de chuva; mantém fog, cores, iluminação, Nether, End e céu |
| Nuvens Realistas | Nuvens 3D volumétricas (mais pesado) |
| Nuvens Arredondadas | Nuvens 3D macias (custo médio) |
| Nuvens em Caixa | Nuvens de blocos estilo vanilla |
| Animação de Chunks | Os blocos sobem ao carregar o terreno |
| Sem Fog e Sem Ondas | Remove névoa e movimento de plantas/água |
| Sem Fog | Remove a névoa do terreno |
| Sem Ondas | Plantas, folhas, lanternas e água paradas |

## Modificações (o que cada uma faz)

- **Fog por bioma** (`tools/gen_fog.py` → `assets/fogs`, `biomes_client.json`): 26 configurações, 81 biomas do Overworld.
  Plains médio e azul-cinza; Forest verde-cinza mais denso; Dark Forest e Swamp muito densos e verdes; Jungle verde-azulado;
  Taiga/Neve/Montanhas frios; Deserto/Mesa/Savana com haze amarelado-bege; Oceano azul com fog subaquática própria por bioma.
  Durante chuva/neve entra a fog de clima (mais densa).
- **Nether:** fog vermelha escura e mais densa (`NL_NETHER_FOG`), ambiente vermelho. Cada bioma do Nether mantém a cor de fog vanilla.
- **End:** curva de fog própria que fecha a visão (`NL_END_FOG`, `NL_END_FOG_START`), céu roxo escuro.
- **Noite** bem mais escura que a vanilla (`NL_NIGHT_DARKNESS`), lua fria e suave (`NL_MOON_INTENSITY`).
- **Sol dramático:** pôr/nascer do sol laranja-avermelhado, contraste maior (`NL_CONTRAST`), saturação menor, sombras frias.
- **Tochas** quentes e com leve tremulação; texturas próprias de tocha, tocha de alma, tocha de redstone e lâmpada de redstone (com brilho).
- **Chuva:** gotas mais inclinadas, céu e nuvens escuros, blocos molhados mais escuros (`NL_WET_DARKEN`), poças que refletem o céu e
  cintilam (`NL_RAIN_SHIMMER`), névoa e neblina de chuva, sol mais fraco. Vale para neve também (fog de clima).
- **Luz do sol entre as folhas** (`NL_LEAF_LIGHT`): sob copas de árvores aparecem manchas de sol que deslizam com o movimento do
  sol e tremem com o vento, com um brilho quente. Só de dia e sem chuva.
- **Aurora a cada 3 dias** (`NL_AURORA_EVERY_DAYS`): aparece na noite de um dia a cada 3 (o jogo fornece o contador de dias).
- **Bloom leve** (`NL_BLOOM_STRENGTH`): é um realce barato de altas luzes, não um bloom de verdade.
- **Água leve:** sem SSR, sem passes extras; tom mais frio e fog subaquática própria.

## Configuração (`src/newb/config.h`, bloco "main tuning knobs")

| Pedido | Macro | Padrão |
|---|---|---|
| FOG_DENSITY | `NL_FOG_DENSITY` | 1.0 |
| FOG_DISTANCE | `NL_FOG_DISTANCE` | 0.85 |
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
| (novo) tinta por bioma | `NL_BIOME_TINT` | 0.6 |
| (novo) luz nas folhas | `NL_LEAF_LIGHT` | 0.9 |
| (novo) brilho das poças | `NL_RAIN_SHIMMER` | 0.3 |
| (novo) aurora a cada N dias | `NL_AURORA_EVERY_DAYS` | 3.0 |

Para mudar a fog de um bioma: edite a tabela `GROUPS` em `tools/gen_fog.py` e rode `python3 tools/gen_fog.py`.
Ícone e texturas próprias: `tools/gen_icon.py`, `tools/gen_textures.py` (precisam de Pillow e numpy).

## Compatibilidade com 26.50

O formato dos materiais mudou no 1.26.50. Igual ao upstream (`devendrn/newb-x-mcbe`, PR #108): `lazurite==0.10.0`,
fontes de material `src-materials-1.26.51.zip`, `min_supported_mc_version = [1, 26, 50]` e uma linha removida de `SunMoon/vertex.sc`.

## Como compilar

**GitHub Actions (recomendado, funciona só com o celular):**

```bash
read -rs GH_TOKEN; export GH_TOKEN     # cola o token (não aparece na tela)
bash tools/publish.sh                  # cria/atualiza o repo, envia o código e dispara o build
```

Depois: Actions > build > artifact `Newb-X-Dark-Manager-android` (o `.mcpack`).
Token: classic com escopos `repo` + `workflow`.

**Local (PC/Linux):** `pip install -r requirements.txt && ./build.sh setup && ./build.sh pack -p android -v 1`.

Verificações: `python3 tools/check_static.py` (includes, macros, fogs, biomas, texturas, todas as variantes de subpack)
e, depois do build, `python3 tools/validate_pack.py --profile android` (material.bin de cada modo, manifest, estrutura do mcpack).

## Limitações (ditas com exatidão)

- Nada disto foi testado dentro do jogo; valores de fog, cor e luz das folhas precisam de ajuste fino no aparelho.
- O mapeamento dos valores de fog do JSON para o shader é assumido igual ao que o Newb já usa para o End; se o jogo o transformar,
  a densidade por bioma sai diferente da prevista (mexer nos números de `gen_fog.py`). Se o jogo ignorar `fog_identifier` em `biomes_client.json`,
  o shader cai na fog única do Overworld (`NL_FOG_DISTANCE`), sem quebrar nada.
- A aurora usa o uniform `Day` do jogo (contador de dias). Se ele não vier como esperado, a aurora aparece sempre ou nunca.
- Bloom real não existe em materiais (sem passe de pós-processamento).
- **Efeito de portais: não implementado.** Precisa de textura/partícula do portal, e o shader não identifica o bloco de portal.
- A luz nas folhas é por vértice (blocos de 1x1): as manchas têm tamanho de bloco, não são raios volumétricos.

## Créditos e licença

Baseado em Newb X Legacy / Newb Shader (devendrn e colaboradores), código-fonte sob licença MIT.
Texturas de tochas/redstone e ícone: criados do zero para este projeto. **Nenhum arquivo dos packs "La Nature by Carrot" ou
"RG Shader" foi usado**: ambos proíbem reuso de assets sem permissão escrita/direta dos autores.
