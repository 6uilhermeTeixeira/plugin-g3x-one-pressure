# Arquitetura

## Fluxo de áudio

```text
entrada finita → Input suavizado → detector peak/RMS linkado + HPF 80 Hz
              → compressor program-dependent → dry/wet → saturação → makeup → saída
```

O `PressureEngine` não aloca, bloqueia, registra logs nem acessa a UI durante o
processamento. `amount` e o ganho de Input são suavizados em 25 ms. As medições
são publicadas em atomics e lidas pela interface a 45 Hz.

## Mapeamento do macro

Pressure normaliza 0–10 para 0–1 e deriva:

- threshold de 0 a aproximadamente -38 dB;
- ratio de 1:1 a 20:1;
- attack de 25 a 1 ms;
- release de 250 a 45 ms, acelerado conforme a redução;
- knee de 12 a 2 dB;
- mistura paralela, makeup limitado a 8 dB e saturação no terço final.

Em zero, ratio, mix e makeup são forçados ao estado neutro para o null test. O
detector usa peak/RMS e um HPF de 80 Hz para reduzir pumping excessivo causado
por graves, mantendo link estéreo.

## Estado e compatibilidade

Os IDs públicos são `amount` e `inputMode`. O estado usa APVTS/ValueTree,
carrega `stateVersion = 1` e os seis presets são expostos como programas ao host.

## Limitações da alpha

- Saturação zero-latency, sem oversampling.
- Sem limiter/lookahead; níveis extremos podem ultrapassar 0 dBFS.
- Sidechain externo, multibanda, mid/side, AAX e iOS permanecem fora do escopo.
