#!/bin/bash

# Script para executar a simulação do ESP32-S3 no Renode
# Uso: ./scripts/run_renode_simulation.sh [single|mesh] [firmware_path]

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SIMULATION_TYPE="${1:-single}"
FIRMWARE_PATH="${2:-$PROJECT_ROOT/dist/esp32.bin}"

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Escolher script baseado no tipo
if [ "$SIMULATION_TYPE" = "single" ]; then
    RENODE_SCRIPT="$PROJECT_ROOT/renode/esp32s3-main.resc"
    echo -e "${GREEN}🎯 Iniciando simulação SINGLE NODE do ESP32-S3${NC}"
elif [ "$SIMULATION_TYPE" = "mesh" ]; then
    RENODE_SCRIPT="$PROJECT_ROOT/renode/mesh-sim.resc"
    echo -e "${GREEN}🌐 Iniciando simulação MESH (5 nós) do ESP32-S3${NC}"
else
    echo -e "${RED}❌ Tipo de simulação inválido: $SIMULATION_TYPE${NC}"
    echo "   Opções: single, mesh"
    exit 1
fi

echo "Firmware: $FIRMWARE_PATH"
echo "Script:   $RENODE_SCRIPT"
echo ""

# Verificar se o firmware existe
if [ ! -f "$FIRMWARE_PATH" ]; then
    echo -e "${RED}❌ Erro: Firmware não encontrado em $FIRMWARE_PATH${NC}"
    echo ""
    echo "   Como compilar:"
    echo "   1. docker compose up -d --build"
    echo "   2. docker cp motoserra-detect-system-guardian-build:/build-area/build/esp32s3.bin dist/esp32.bin"
    echo ""
    exit 1
fi

# Verificar se o Renode está instalado
if ! command -v renode &> /dev/null; then
    echo -e "${RED}❌ Erro: Renode não instalado${NC}"
    echo ""
    echo "   Instale com:"
    echo "   pip install renode"
    echo ""
    exit 1
fi

# Verificar NumPy para síntese de áudio
if ! python3 -c "import numpy" 2>/dev/null; then
    echo -e "${YELLOW}⚠️  NumPy não instalado (necessário para síntese de áudio)${NC}"
    echo "   Instale com: pip install numpy"
    echo ""
fi

# Executar Renode
echo -e "${GREEN}Iniciando Renode...${NC}"
echo ""

cd "$PROJECT_ROOT"

if [ "$SIMULATION_TYPE" = "single" ]; then
    echo -e "${YELLOW}💡 Dicas:${NC}"
    echo "  start                        - Iniciar simulação"
    echo "  inject_synthetic_tone        - Injetar tom de teste"
    echo "  inject_chainsaw              - Injetar som de motosserra"
    echo "  print_energy_status          - Ver consumo de energia"
    echo "  simulate_chainsaw_detection  - Cenário completo"
    echo "  quit                         - Sair"
    echo ""
elif [ "$SIMULATION_TYPE" = "mesh" ]; then
    echo -e "${YELLOW}💡 Dicas:${NC}"
    echo "  start                      - Iniciar todos os 5 nós"
    echo "  simulate_mesh_detection    - Propagar detecção por mesh"
    echo "  print_mesh_status          - Status da rede"
    echo "  print_energy_status        - Energia de todos"
    echo "  write_energy_reports       - Salvar relatórios"
    echo "  quit                       - Sair"
    echo ""
fi

renode "$RENODE_SCRIPT"
