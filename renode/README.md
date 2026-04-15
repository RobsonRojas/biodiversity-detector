# 🎯 Guia Completo: Simulação do ESP32-S3 no Renode

## 📋 Visão Geral das Melhorias Implementadas

Implementei 4 melhorias principais na simulação:

### 1. ✅ **Mock de Sensores I2S** (`esp32s3.repl`)
- Simula interface de áudio I2S (PDM microphone)
- Buffer FIFO de 64KB para captura de áudio
- Região de metadados para comunicação com firmware

### 2. ✅ **Simulação LoRa em Mesh** (`mesh-sim.resc`)
- 5 nós simulados (1 Gateway + 2 Routers + 2 Sensores)
- Cada nó em porta UART diferente
- Simulação de propagação de pacotes LoRa

### 3. ✅ **Injeção de Áudio Real** (`audio_mock_script.py`)
- Carregamento de arquivos WAV reais
- Suporte a síntese de áudio (tons, frequências)
- Rastreamento de múltiplas injeções
- Metadados de amostragem (sample rate, bit depth)

### 4. ✅ **Monitor de Energia** (`energy_monitor.py`)
- Rastreamento de consumo em diferentes estados
- Simulação de bateria (mAh)
- Simulação de RSSI (sinal)
- Relatórios detalhados

---

## 🚀 Como Usar

### **Instalação de Dependências**

```bash
# Instalar Renode
pip install renode

# Instalar NumPy para síntese de áudio
pip install numpy
```

### **Simulação Simples (Nó Único)**

```bash
# 1. Compilar firmware
docker compose up -d --build

# 2. Copiar binário compilado
docker cp motoserra-detect-system-guardian-build:/build-area/build/esp32s3.bin dist/esp32.bin

# 3. Executar simulação
cd /media/rob/windows5/git/motoserra-detect-system
renode renode/esp32s3-main.resc
```

**Dentro da console Renode:**

```renode
start
inject_chainsaw
print_energy_status
write_energy_report
quit
```

### **Simulação em Mesh (5 Nós)**

```bash
cd /media/rob/windows5/git/motoserra-detect-system
renode renode/mesh-sim.resc
```

**Dentro da console Renode:**

```renode
start
simulate_mesh_detection
print_mesh_status
print_energy_status
write_energy_reports
quit
```

---

## 📊 Comandos Disponíveis

### **Simples (esp32s3-main.resc)**

| Comando | Descrição |
|---------|-----------|
| `start` | Inicia a simulação |
| `pause` | Pausa a simulação |
| `inject_chainsaw` | Injeta áudio de motosserra |
| `inject_frog` | Injeta som de sapo |
| `inject_synthetic_tone` | Injeta tom de 440Hz |
| `set_state_sleep` | Simula deep sleep |
| `set_state_active` | Simula CPU ativo |
| `set_state_lora_tx` | Simula transmissão LoRa |
| `print_audio_status` | Status do injetor de áudio |
| `print_energy_status` | Status de energia |
| `print_all_status` | Todo status |
| `simulate_chainsaw_detection` | Cenário completo |
| `write_energy_report` | Salva relatório em arquivo |

### **Mesh (mesh-sim.resc)**

| Comando | Descrição |
|---------|-----------|
| `start` | Inicia todos os 5 nós |
| `pause` | Pausa simulação |
| `trigger_chainsaw_on_node1` | Injeta detecção no Node 1 |
| `print_mesh_status` | Status da rede mesh |
| `print_energy_status` | Energia de todos os nós |
| `simulate_mesh_detection` | Propaga detecção por mesh |
| `write_energy_reports` | Salva relatórios de todos nós |

---

## 📊 Exemplos de Uso

### **Exemplo 1: Teste Simples de Detecção**

```renode
(esp32s3-main.resc)
start
set_state_active
inject_synthetic_tone
sleep 2
print_energy_status
quit
```

**Saída esperada:**
```
[Energy] === Energy Monitor Status ===
  Machine: ESP32S3_Node1
  Battery: 99.5% (4975.00 / 5000 mAh)
  Current State: idle (1.0 mA)
  RSSI: -80 dBm
  Total Consumed: 0.25 mAh
```

### **Exemplo 2: Cenário Completo de Detecção**

```renode
(esp32s3-main.resc)
start
simulate_chainsaw_detection
print_all_status
write_energy_report
quit
```

Gera relatório: `energy_report.txt`

### **Exemplo 3: Simulação Mesh com Propagação**

```renode
(mesh-sim.resc)
start
simulate_mesh_detection
print_mesh_status
write_energy_reports
quit
```

Gera relatórios para cada nó:
- `energy_reports/Gateway_Node5_energy.txt`
- `energy_reports/Router_Node4_energy.txt`
- `energy_reports/Router_Node3_energy.txt`
- `energy_reports/Router_Node2_energy.txt`
- `energy_reports/Leaf_Node1_energy.txt`

---

## 🔧 Estrutura de Arquivos

```
renode/
├── esp32s3-main.resc          # Simulação de nó único
├── esp32s3.repl               # Plataforma ESP32-S3 com I2S, GPIO, ADC
├── mesh-sim.resc              # Simulação de 5 nós em mesh
├── audio_mock_script.py       # Injetor de áudio com síntese
└── energy_monitor.py          # Monitor de energia e bateria

dist/
└── esp32.bin                  # Binário compilado

assets/mocks/                  # (Opcional) Áudios reais
├── chainsaw_mock.wav
└── frog_call.wav
```

---

## ⚡ Estados de Potência Simulados

```python
'deep_sleep': 0.01 mA       # 10 µA
'light_sleep': 0.1 mA       # 100 µA
'idle': 1.0 mA              # 1 mA
'cpu_active': 50.0 mA       # 50 mA
'audio_capture': 10.0 mA    # 10 mA (PDM)
'wifi_active': 100.0 mA     # 100 mA
'lora_tx': 120.0 mA         # 120 mA
'lora_rx': 30.0 mA          # 30 mA
```

---

## 📈 Interpretando Relatórios de Energia

### **Exemplo de Relatório:**

```
Energy Monitor Report
Machine: Leaf_Node1
Timestamp: 2026-04-15T14:23:45.123456

Battery Status:
  Current: 95.2%
  Remaining: 4760.00 mAh
  Capacity: 5000 mAh
  Total Consumed: 240.00 mAh

Power State Breakdown:
  lora_tx             :   150.000 mAh (62.5%)
  audio_capture       :    60.000 mAh (25.0%)
  cpu_active          :    20.000 mAh (8.3%)
  idle                :    10.000 mAh (4.2%)

State Transitions:
  2026-04-15T14:23:10 idle            → audio_capture   (10.000 mAh)
  2026-04-15T14:23:12 audio_capture   → lora_tx         (120.000 mAh)
  2026-04-15T14:23:14 lora_tx         → idle            (50.000 mAh)
```

### **Interpretação:**

- **Battery Status**: Nível atual, consumo total
- **Breakdown**: Quais estados consumiram mais energia
- **Transitions**: Histório de mudanças de estado

---

## 🎓 Casos de Uso Avançados

### **1. Teste de Autonomia**

```python
# Executar simulação por tempo longo e verificar quando bateria acaba
# Útil para estimar tempo de operação em campo
```

### **2. Otimização de Consumo**

```python
# Comparar consumo de diferentes estratégias de sleep/wake
# Ajustar ciclos para maximizar autonomia
```

### **3. Simulação de Interferência RF**

```renode
python "
router2_energy.simulate_interference(0.7)  # 70% interferência
router2_energy.print_status()
"
```

### **4. Teste de Mobilidade**

```renode
python "
leaf1_energy.simulate_motion(0.8)  # Node se move bastante
leaf1_energy.print_status()
"
```

---

## 🐛 Troubleshooting

| Problema | Solução |
|----------|---------|
| `ModuleNotFoundError: No module named 'numpy'` | `pip install numpy` |
| `WAV file not found` | Verificar caminho relativo ou absoluto |
| `Port already in use` | Alterar porta em `.resc` (45679 → 45700, etc) |
| `Binary not found` | Executar `docker compose up -d --build` primeiro |
| `UART connection refused` | Aguardar alguns segundos após `start` |

---

## 📝 Próximos Passos Recomendados

1. **Adicionar Áudios Reais**: Copiar WAV de motosserra/animais para `assets/mocks/`
2. **Calibrar Consumo**: Ajustar mA por estado baseado em datasheet real
3. **Simular Clima**: Adicionar efeitos de temperatura/umidade
4. **Análise de Bateria**: Implementar curva de descarga mais realista
5. **Visualização**: Criar gráficos de consumo usando matplotlib

---

## 📚 Referências

- **Renode Docs**: https://renode.readthedocs.io/
- **ESP32-S3 Datasheet**: https://www.espressif.com/products/socs/esp32-s3/
- **TensorFlow Lite Micro**: https://github.com/tensorflow/tflite-micro

---

## 💡 Dicas Práticas

✅ **Sempre compilar antes de simular:**
```bash
docker compose up -d --build
docker cp <container_id>:/build-area/build/esp32s3.bin dist/esp32.bin
```

✅ **Usar scripts em loop:**
```bash
# Executar múltiplas detecções automaticamente
for i in {1..10}; do
    echo "inject_synthetic_tone" | renode renode/esp32s3-main.resc
done
```

✅ **Monitorar output serial em tempo real:**
```bash
# Terminal 1
nc localhost 45679

# Terminal 2
renode renode/esp32s3-main.resc
```

---

Pronto! Sua simulação do ESP32-S3 está completa com **audio injection, mesh networking, e energy monitoring**! 🎉
