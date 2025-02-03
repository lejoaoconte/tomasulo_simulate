# Para rodar

- Para compilar

```bash
gcc -Wall -Wextra -std=c11 -I./src main.c src/tomasulo.c src/files.c -o main
```

- Arquivo teste, exemplo:

```assembly
add R1, R2, R3
sub R4, R1, R2
lw R5, 0(R1)
sw R5, 4(R1)
```

# Explicação baseado no arquivo base

> **Nota:**
>
> - Todos os registradores (R0 a R31) iniciam com o valor `0` e sem dependências (campo `tag = -1`).
> - A memória inicia com o valor `0` em todas as posições.
> - As estações de reserva (RS0 a RS5) estão inicialmente livres.

---

## Ciclo de Clock 0 – Fase de Issue (Emissão)

Cada instrução é lida e emitida para uma estação de reserva, onde os operandos são analisados e armazenados conforme sua disponibilidade.

### 1. Instrução: `add R1, R2, R3`

- **Alocação:**  
  A instrução é alocada na **RS0**.
- **Operandos:**
  - **R2:** Está pronto (tag = -1), então seu valor (0) é copiado para o campo `Vj`.
  - **R3:** Está pronto, então seu valor (0) é copiado para o campo `Vk`.
- **Destino:**
  - O registrador **R1** é marcado para aguardar o resultado (seu `tag` é atualizado para 0, identificando a RS0).
- **Latência:**
  - Como é uma operação ADD, o ciclo de execução é definido como `0 + 2 = 2`.

---

### 2. Instrução: `sub R4, R1, R2`

- **Alocação:**  
  A instrução é alocada na **RS1**.
- **Operandos:**
  - **R1:** Ainda não está pronto (foi marcado pela RS0); portanto, a RS1 registra `Qj = 0` (indicando dependência da RS0).
  - **R2:** Está pronto, e seu valor (0) é copiado para `Vk`.
- **Destino:**
  - O registrador **R4** é marcado para aguardar o resultado (seu `tag` é atualizado para 1, identificando a RS1).
- **Latência:**
  - A execução será no ciclo `0 + 2 = 2`.

---

### 3. Instrução: `lw R5, 0(R1)`

- **Alocação:**  
  A instrução é alocada na **RS2**.
- **Operando (Base):**
  - Usa **R1** como base; como R1 não está pronto (tag = 0), a RS2 registra `Qj = 0`.
- **Destino:**
  - O registrador **R5** é marcado para aguardar o resultado (tag atualizado para 2, identificando a RS2).
- **Latência:**
  - A execução será no ciclo `0 + 3 = 3`.

---

### 4. Instrução: `sw R5, 4(R1)`

- **Alocação:**  
  A instrução é alocada na **RS3**.
- **Operandos:**
  - **Valor a armazenar:** Vem de **R5**. Como R5 não está pronto (tag = 2), RS3 registra `Qj = 2`.
  - **Base para o endereço:** Usa **R1**; como R1 não está pronto (tag = 0), RS3 registra `Qk = 0`.
- **Destino:**
  - Não há destino de registrador para SW, pois a operação escreve na memória.
- **Latência:**
  - A execução será no ciclo `0 + 3 = 3`.

---

**Estado ao final do Ciclo 0:**

- **Estações de Reserva:**
  - **RS0:** Preparada para executar `add` no ciclo 2, com `Vj = 0` e `Vk = 0`.
  - **RS1:** Preparada para executar `sub`, aguardando o valor de R1 (`Qj = 0`) e com R2 pronto.
  - **RS2:** Preparada para executar `lw`, aguardando o valor de R1 (`Qj = 0`).
  - **RS3:** Preparada para executar `sw`, aguardando o valor de R5 (`Qj = 2`) e de R1 (`Qk = 0`).
- **Registradores:**
  - **R1, R4, R5:** Marcados para aguardar os resultados (com tags 0, 1 e 2, respectivamente).

---

## Ciclo de Clock 1 – Fase de Espera

- **Nenhuma estação atinge seu ciclo de execução (exec_cycle == 1).**
- **Estado:**  
  O sistema aguarda e o estado (RS, registradores e memória) permanece inalterado.

---

## Ciclo de Clock 2 – Execução do ADD e SUB

### RS0 – Executando `add R1, R2, R3`

- **Operação:**
  - Calcula `Vj + Vk = 0 + 0 = 0`.
- **Write-back:**

  - O resultado 0 é escrito no registrador **R1**, que estava aguardando esse valor (R1 tinha tag = 0).
  - R1 é atualizado para valor 0 e seu `tag` passa a ser -1 (pronto).

- **Propagação:**

  - O resultado é propagado para todas as estações de reserva que aguardavam o valor produzido pela RS0 (por exemplo, RS1, RS2 e RS3).

- **Liberação:**
  - A RS0 é liberada (busy = 0).

---

### RS1 – Executando `sub R4, R1, R2`

- **Operação:**
  - Calcula `Vj - Vk = 0 - 0 = 0`.
- **Write-back:**

  - O resultado 0 é escrito no registrador **R4** (R4 tinha tag = 1).
  - R4 é atualizado para valor 0 e seu `tag` passa a ser -1.

- **Propagação:**

  - O resultado é propagado para outras estações, se necessário.

- **Liberação:**
  - A RS1 é liberada.

---

**Estado ao final do Ciclo 2:**

- **Estações de Reserva:**
  - RS0 e RS1 estão livres.
  - RS2 e RS3 já tiveram seus campos relacionados a R1 atualizados (pela propagação do resultado da RS0).
- **Registradores:**
  - **R1** e **R4** agora têm valor 0 e estão prontos (tag = -1).
  - **R5** ainda aguarda o resultado da RS2 (tag = 2).

---

## Ciclo de Clock 3 – Execução do LW e SW

### RS2 – Executando `lw R5, 0(R1)`

- **Cálculo do Endereço:**
  - É calculado somando o valor base (`Vj` de R1, que agora é 0) com o imediato: `0 + 0 = 0`.
- **Operação:**
  - Lê o valor da memória na posição 0, que é 0.
- **Write-back:**
  - O resultado 0 é escrito no registrador **R5**, que estava aguardando esse valor (R5 tinha tag = 2).
  - R5 é atualizado para valor 0 e seu `tag` passa a ser -1.
- **Propagação:**
  - O resultado é propagado para as estações que aguardavam (por exemplo, na RS3, que dependia de R5).
- **Liberação:**
  - A RS2 é liberada.

---

### RS3 – Executando `sw R5, 4(R1)`

- **Cálculo do Endereço:**
  - Calcula-se o endereço efetivo somando o valor base (`Vk` de R1, que é 0) com o imediato: `0 + 4 = 4`.
- **Operação:**
  - Armazena o valor de `Vj` (que é o valor de R5, 0) na memória na posição 4.
- **Liberação:**
  - A RS3 é liberada.

---

**Estado Final ao Término do Ciclo 3:**

- **Estações de Reserva:**  
  Todas estão livres.
- **Registradores:**
  - **R1, R4 e R5** possuem valor 0 e estão prontos (tag = -1).
- **Memória:**
  - Posição 0: Valor 0 (resultado do LW).
  - Posição 4: Valor 0 (resultado do SW).
  - Como todos os valores iniciais eram 0, visualmente não há alteração, mas o fluxo de dados e a resolução das dependências ocorreram conforme esperado.

---

## Resumo Geral da Execução

1. **Fase de Issue (Ciclo 0):**

   - As instruções são lidas e alocadas em estações de reserva.
   - Os operandos são verificados:
     - Se o operando está pronto, seu valor é copiado para os campos `Vj`/`Vk`.
     - Caso contrário, a estação registra a tag da RS que produzirá o valor (ex.: RS0 para R1).
   - Os registradores de destino (R1, R4, R5) são marcados como aguardando o resultado.

2. **Fase de Espera (Ciclo 1):**

   - O sistema aguarda até que o ciclo de clock atinja o ciclo de execução definido pela latência.

3. **Execução e Write-back (Ciclos 2 e 3):**

   - **Ciclo 2:**
     - RS0 (ADD) executa e escreve o resultado em R1, propagando o valor para as RS dependentes.
     - RS1 (SUB) executa e escreve o resultado em R4.
   - **Ciclo 3:**
     - RS2 (LW) executa, lendo a memória e atualizando R5.
     - RS3 (SW) executa, calculando o endereço e armazenando o valor na memória.

4. **Final da Simulação:**
   - Todas as estações de reserva estão livres, indicando que todas as instruções foram executadas.
   - O estado final dos registradores e da memória reflete a execução correta do algoritmo.

---

## Conclusão

Este passo a passo demonstra como o algoritmo de Tomasulo gerencia:

- **Issue:** A emissão e alocação das instruções com verificação de dependências.
- **Execução:** O processamento das operações conforme as latências definidas.
- **Write-back e Propagação:** A atualização dos registradores e a propagação dos resultados para resolver dependências.

Mesmo que os valores finais permaneçam 0 neste cenário de teste, o fluxo de controle e a resolução das dependências evidenciam a essência do algoritmo de Tomasulo.
