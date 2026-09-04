# G3X One Pressure

Compressor de caráter com um único macrocontrole, desenvolvido em C++20, JUCE
8.0.8 e CMake. Produz VST3 e aplicativo Standalone com DSP, interface e presets
originais baseados nos requisitos do [PRD](PRD.md).

## Recursos

- Macro Pressure 0–10: compressão paralela leve, punch, pumping e squash.
- Input em três estados: Pad (-6 dB), Normal e Drive (+6 dB).
- Detector peak/RMS estéreo linkado com sidechain HPF em 80 Hz.
- Mapeamento contínuo de threshold, ratio, attack, release, knee e dry/wet.
- Makeup controlado e saturação suave no terço final da escala.
- Identidade exata em Pressure 0 com Input Normal.
- Medidor de gain reduction e LEDs de pico de entrada e saída.
- Automação suave, estado versionado e seis presets expostos ao host.
- Interface compacta, redimensionável, HiDPI e acessível por teclado.

## Compilar

Pré-requisitos: CMake 3.22+, compilador C++20 e Git. O JUCE é obtido em versão
fixada pelo CMake.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

No Linux, instale ALSA, FreeType, Fontconfig e OpenGL. Os artefatos ficam em
`build/G3XOnePressure_artefacts/Release/`. No Windows, use Visual Studio/MSVC e uma
configuração Release de 64 bits.

## Parâmetros

| Parâmetro | Valores | Padrão |
|---|---|---|
| Pressure | 0.0–10.0 | 0.0 |
| Input | Pad / Normal / Drive | Normal |

## Presets

Neutral, Drum Glue, Parallel Punch, Room Pump, Loop Smash e Aggressive Bus.

## Estado

Alpha funcional, concluída até M3 e preparada para M4. O próximo passo é baixar
o artefato Windows da CI e validar manualmente VST3, automação, restauração de
sessão e áudio no FL Studio antes de considerar M4 concluído.

## Independência

O produto Waves foi consultado apenas como referência de categoria e ergonomia.
O G3X One Pressure não reutiliza marca, assets, interface, presets ou algoritmo da
referência. Veja [fontes e limites de uso](docs/references/README.md).

## Licença

Distribuído sob a [licença MIT](LICENSE).

## Download e instalação — Windows x64

1. Abra [Actions](https://github.com/6uilhermeTeixeira/plugin-g3x-one-pressure/actions) e selecione uma execução bem-sucedida da branch `main`.
2. Em **Artifacts**, baixe `G3X-One-Pressure-Windows-x64-<commit>`. O download fica disponível por 30 dias; **Run workflow** permite gerar um novo build.
3. Extraia o ZIP. A raiz contém somente `SHA256SUMS.txt` e a pasta `G3X One Pressure.vst3`, com todos os arquivos internos do plugin.
4. Na pasta extraída, abra o PowerShell e verifique o binário:

```powershell
$expected, $relativePath = (Get-Content -LiteralPath .\SHA256SUMS.txt -Raw).Trim() -split '  ', 2
$actual = (Get-FileHash -LiteralPath $relativePath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actual -ne $expected) { throw "SHA-256 divergente; baixe o artifact novamente." }
"SHA-256 confirmado."
```

5. Copie a pasta **`G3X One Pressure.vst3` inteira** para `C:\Program Files\Common Files\VST3` e atualize a busca de plugins da DAW. A cópia pode solicitar permissão de administrador.

O SHA-256 verifica o binário Windows x64 dentro do bundle; não é o hash do ZIP ou dos recursos. O artifact contém o VST3 Release; o aplicativo Standalone continua disponível como alvo de compilação, mas não é incluído no download.
