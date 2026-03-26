from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI(title="Guardian Web Manager")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
async def root():
    return {"message": "Guardian Web Manager API"}

@app.get("/nodes")
async def get_nodes():
    return [
        {"id": "node-01", "role": "Gateway", "status": "online"},
        {"id": "node-02", "role": "Leaf", "status": "online", "battery": 3800, "rssi": -95},
    ]
