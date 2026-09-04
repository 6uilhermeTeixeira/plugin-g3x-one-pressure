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
