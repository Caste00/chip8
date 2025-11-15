v# CHIP‑8

[Pagina Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

---

## 📦 MEMORY
- **4096 byte** di memoria indirizzabile (`0x000–0xFFF`)
- Ogni indirizzo = 1 byte
- **0x000–0x1FF (0–511):** riservati per interprete/variabili/fontset (80 byte per sprite cifre 0–F)
- **0x200 (512):** punto di inizio standard delle ROM CHIP‑8
- **0xF00–0xFFF:** framebuffer video nei sistemi originali (64×32 bit = 256 byte). Nella mia implementazione ho un buffer separato;

---

## 🗄️ REGISTRI
- **V0 … VF**: 16 registri generali da 8‑bit
  - `VF` usato come flag (carry/borrow, collisione sprite)
- **I**: registro indice (originariamente 12‑bit, oggi 16‑bit)
- **PC**: Program Counter, 16‑bit
- **SP**: Stack Pointer

---

## 📚 STACK
- Contiene indirizzi di ritorno delle subroutine
- Minimo 12 livelli, la mia implementazione ne ha 16
- Implementato come array di `uint16_t`

---

## ⏱️ TIMERS
- **Delay Timer (DT)**: decrementa a 60 Hz fino a 0
- **Sound Timer (ST)**: decrementa a 60 Hz fino a 0, se > 0 emette beep
- Timer a 8‑bit, aggiornati indipendentemente dal ciclo CPU

---

## 🎮 INPUT
- 16 tasti esadecimali (0x0–0xF)
- Layout tipico:

    1 2 3 4  
    q w e r  
    a s d f  
    z x c v  

- 8/4/6/2 usati spesso come direzionali
- Opcode legati:
- `Ex9E`: salta se tasto in `Vx` premuto
- `ExA1`: salta se tasto in `Vx` non premuto
- `Fx0A`: attende pressione di un tasto e lo memorizza in `Vx`

---

## 🖥️ DISPLAY
- Risoluzione: **64 × 32** pixel (monocromatico)
- Ogni pixel = 1 bit
- Sprite: larghezza 8 pixel, altezza 1–15 righe
- Disegno in XOR → pixel accesi possono spegnersi (collisione)
- VF = 1 se almeno un pixel acceso è stato spento durante il draw
- E' possibile scalare lo schermo modificando il campo DISPLAY_SCALE in display.h

---

## 🔊 SUONO
- Beep continuo finché `ST > 0`
- Frequenza e forma d’onda dipendono dall’hardware emulato
- Non implementato, esiste ma non viene emesso nessun suono

---

## 📜 OPCODE CHIP‑8

| Mnemonic | Opcode | Parametri | Descrizione | Note |
|----------|--------|-----------|-------------|------|
| NOP      | `0NNN` | NNN = addr | Chiamata a programma RCA 1802 a NNN | Obsoleto, ignorato negli emulatori moderni, (NOP) |
| CLS      | `00E0` | — | Cancella lo schermo | — |
| RET      | `00EE` | — | Ritorna da subroutine | Pop da stack in PC |
| JP       | `1NNN` | NNN = addr | Salta a NNN | PC = NNN |
| CALL     | `2NNN` | NNN = addr | Chiama subroutine a NNN | Push PC, PC = NNN |
| SE       | `3XNN` | X = reg, NN = byte | Salta prossima istruzione se `Vx == NN` | — |
| SNE      | `4XNN` | X, NN | Salta prossima istruzione se `Vx != NN` | — |
| SEV      | `5XY0` | X, Y | Salta prossima istruzione se `Vx == Vy` | Solo se nibble basso = 0 |
| LD       | `6XNN` | X, NN | `Vx = NN` | — |
| ADD      | `7XNN` | X, NN | `Vx += NN` | No flag carry |
| LDV      | `8XY0` | X, Y | `Vx = Vy` | — |
| OR       | `8XY1` | X, Y | `Vx = Vx OR Vy` | VF non modificato |
| AND      | `8XY2` | X, Y | `Vx = Vx AND Vy` | VF non modificato |
| XOR      | `8XY3` | X, Y | `Vx = Vx XOR Vy` | VF non modificato |
| ADDV     | `8XY4` | X, Y | `Vx += Vy` | VF = carry |
| SUB      | `8XY5` | X, Y | `Vx -= Vy` | VF = NOT borrow |
| SHR      | `8XY6` | X | Shift destro `Vx >>= 1` | VF = bit0 originale |
| SUBN     | `8XY7` | X, Y | `Vx = Vy - Vx` | VF = NOT borrow |
| SHL      | `8XYE` | X | Shift sinistro `Vx <<= 1` | VF = bit7 originale |
| SNEV     | `9XY0` | X, Y | Salta se `Vx != Vy` | Solo se nibble basso = 0 |
| LDI      | `ANNN` | NNN | `I = NNN` | — |
| JPV0     | `BNNN` | NNN | Salta a `V0 + NNN` | — |
| RND      | `CXNN` | X, NN | `Vx = rand() & NN` | rand = 8‑bit casuale |
| DRW      | `DXYN` | X, Y, N | Disegna sprite 8×N a `(Vx, Vy)` da memoria[I], in modalità schip con n = 0, disegna uno sprite 16x16 | VF = collisione |
| SKP      | `EX9E` | X | Salta se tasto `Vx` premuto | — |
| SKNP     | `EXA1` | X | Salta se tasto `Vx` NON premuto | — |
| LD_DT    | `FX07` | X | `Vx = Delay Timer` | — |
| LD_K     | `FX0A` | X | Attende pressione tasto → `Vx` | Bloccante |
| LD_DT_SET| `FX15` | X | `Delay Timer = Vx` | — |
| LD_ST    | `FX18` | X | `Sound Timer = Vx` | — |
| ADD_I    | `FX1E` | X | `I += Vx` | VF non modificato |
| LD_F     | `FX29` | X | `I = addr sprite cifra Vx` | Font 4×5 |
| LD_B     | `FX33` | X | BCD di `Vx` in memoria[I..I+2] | Centinaia, decine, unità |
| LD_MEM   | `FX55` | X | Salva V0..Vx in memoria[I..I+X] | I++ dopo ogni store (varia) |
| LD_REG   | `FX65` | X | Carica V0..Vx da memoria[I..I+X] | I++ dopo ogni load (varia) |
