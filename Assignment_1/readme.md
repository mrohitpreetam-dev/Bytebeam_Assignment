This project simulates a vehicle telemetry logger.

Every cycle:
- Reads GPS location
- Reads ignition state
- Reads sensor data
- Creates a telemetry packet
- Stores packet in a ring buffer
- Transmits packet to the server
- If the network is unavailable, packets remain buffered.
- When connectivity returns, buffered packets are transmitted in FIFO order.

gps.c
    Simulated GPS interface

daq.c
    Simulated sensor acquisition

peripherial.c
    Simulated ignition input

server.c
    Simulated network/server interface

main.c
    Application logic
    Packet creation
    Buffer management
    Transmission handling