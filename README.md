# Simulation Parameters

This file outlines the parameters used for the simulation.

## General

| Parameter          | Value                                       |
|---------------------|----------------------------------------------|
| Number of UEs      | 10 per eNB; 1 Downlink UDP Flow per UE        |
| Number of eNBs     | 4                                            |
| Inter-eNB distance | 5 km                                         |
| eNB Tx Power       | 40 dBm (10 Watt)                              |
| Application Type   | UDP                                          |
| Simulation Time    | 30 seconds                                  |
| Number of Seeds    | 5                                            |
| Seed Range         | RngRun1 to RngRun5 (based on last two digits of roll number) |

## Traffic Cases

**Full Buffer Case:**

- UDP traffic: 1500 bytes every 1 ms
- DL UDP flow per UE: 12 Mbps

**Non-Full Buffer Case:**

- UDP traffic: 1500 bytes every 10 ms
- DL UDP flow per UE: 1.2 Mbps

## Mobility

- UE mobility speeds: 0, 10 m/s
- Mobility model: RandomWalk2d
- UE placement: Random disc within 500m radius of eNB

## Radio Resources

- Number of RBs: 50 in DL and 50 in UL (LTE FDD)

## UE Attachment

- Automatic attachment to eNB based on received signal strength
- Handovers possible during mobility

## Seed Generation

- RngRun1: Use the last two digits of your roll number
- RngRun5: RngRun1 + 4
- Generate seeds sequentially for each experiment
