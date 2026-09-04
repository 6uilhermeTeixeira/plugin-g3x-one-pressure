# G3X Pressure — Product Requirements Document

**Versão:** 0.1.0  
**Status:** alpha funcional; validação auditiva e de hosts pendente
**Target:** C++20, JUCE fixado e CMake  
**Entrega inicial:** VST3 64-bit para Windows; Standalone para desenvolvimento

## 1. Visão

G3X Pressure é um compressor de caráter para bateria, loops, buses e fontes
dinâmicas. Um macrocontrole percorre compressão paralela leve, punch, pumping e
squash agressivo, preservando rapidez de uso e uma assinatura sonora própria.

A documentação do Waves OneKnob Pressure informa esse arco funcional e um
controle de entrada Pad/Boost. Não há especificação pública do algoritmo; a
curva, constantes e saturação G3X serão projetadas e testadas do zero.

## 2. Objetivos

- Entregar compressão musical com uma única decisão principal.
- Manter transientes nos ajustes baixos e criar pumping intencional nos altos.
- Adaptar fontes fracas ou fortes sem alterar o caráter do macrocontrole.
- Operar em mono e estéreo linkado, com automação suave.
- Ser seguro em tempo real e compatível com sessões futuras.

## 3. Controles públicos

### Pressure (`amount`)

- Faixa exibida: 0.0–10.0; padrão 0.0; normalização interna 0–1.
- 0 deve ser neutro; 1–3 compressão paralela leve; 4–7 punch/pumping; 8–10
  squash, densidade e caráter agressivo.
- O macro pode mapear threshold, ratio, attack, release, mix e saturação.

### Input (`inputMode`)

- Três estados próprios propostos: `Pad` (-6 dB), `Normal` (0 dB) e `Drive`
  (+6 dB), com Normal como padrão.
- Mudança suavizada e aplicada antes do detector e do caminho de áudio.

Os IDs tornam-se contratos após o primeiro beta.

## 4. DSP proposto

```text
Input -> trim -> detector estéreo linkado -> compressor program-dependent
      -> caminho seco + caminho comprimido/saturado -> makeup -> Output
```

- Detector combinado peak/RMS, sidechain com HPF inicial de 80 Hz.
- Knee suave nos ajustes baixos e progressivamente mais firme nos altos.
- Attack inicial varia aproximadamente de 25 ms a 1 ms.
- Release varia aproximadamente de 250 ms a 45 ms e pode tornar-se
  program-dependent após validação.
- Ratio efetivo cresce de cerca de 2:1 até limitação agressiva.
- Mistura paralela preserva transientes no início da escala e converge para o
  caminho processado nos valores altos.
- Saturação suave original pode crescer no terço final, sujeita a testes de
  aliasing e loudness-matched A/B.
- Os números são hipóteses G3X, não alegações sobre o produto Waves.

## 5. Interface

- Knob central 0–10, seletor Input e valor numérico.
- Medidor compacto de redução e LEDs de pico de entrada/saída.
- Janela compacta, redimensionável, HiDPI e acessível por teclado.
- Visual G3X próprio; madeira, metal, logotipo, ícone e proporções da referência
  não serão reutilizados.

## 6. Requisitos de tempo real

- Sem alocações, locks, I/O ou UI em `processBlock`.
- 44.1–192 kHz; buffers de 16–2048 samples; zero latência adicionada como alvo.
- Proteção contra NaN, Inf e denormals; parâmetros suavizados.
- Estado serializado e versionado; processamento mono e estéreo linkado.

## 7. Testes e aceitação

- Identidade/null test em `amount = 0`.
- Curvas estáticas, attack/release, mix paralelo e ganho máximo testados.
- Transientes, pumping e distorção medidos nos pontos 0, 2.5, 5, 7.5 e 10.
- Sem clicks, canais divergentes ou saída não finita.
- Builds Linux Debug/Release e Windows VST3 Release via MSVC.
- Estado recuperado pelo host; pluginval/VST3 Validator antes do beta.
- Validação em FL Studio com bateria, loops e bus de drums.

## 8. Presets iniciais

- Neutral, Drum Glue, Parallel Punch, Room Pump, Loop Smash e Aggressive Bus.

## 9. Fora do escopo

- Clonagem sample-a-sample ou engenharia reversa da Waves.
- Assets, marca, presets ou trade dress da Waves.
- Sidechain externo, multibanda, mid/side, AAX e iOS no primeiro release.

## 10. Marcos

1. **M0:** PRD, naming, licença e arquitetura.
2. **M1:** detector, curva estática e testes.
3. **M2:** envelope, paralelo, saturação e estéreo.
4. **M3:** interface, medidores, presets e acessibilidade.
5. **M4:** CI Windows, VST3 e teste no FL Studio.
6. **M5:** validadores, regressão e beta.

## 11. Decisões para confirmação

- Nome `G3X Pressure`; manter Input em três estados ou apenas Pad/Boost.
- Saturação audível nos valores altos ou compressão limpa.
- Exibir redução de ganho ou manter visual literalmente minimalista.
- Zero latência absoluto ou modo opcional com lookahead.

## 12. Fontes

- [Produto oficial](https://www.waves.com/plugins/oneknob-pressure)
- [Manual oficial OneKnob](https://assets.wavescdn.com/pdf/plugins/oneknob-series.pdf)
- [Imagem oficial](https://media.wavescdn.com/images/products/plugins/600/oneknob-pressure.png)

Consulta em 4 de setembro de 2026; fontes usadas apenas como referência.
