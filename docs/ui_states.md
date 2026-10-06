# States and Modes
```
UiState
├── Main
│   ├── Mode:Brief
│   │   ├── Idle
│   │   ├── Running
│   │   ├── Auto
│   │   ├── Waiting
│   │   ├── Stopped
│   │   ├── Finished
│   │   └── Error
│   │
│   └── Mde:Detailed
│       └── Running → Monitor
│
├── ProfileSelection
│   ├── Mode:Start
│   └── Mode:Edit
│
├── Settings
│   ├── Mode:PID
│   └── Mode:Other
│
├── Question
│   ├── Mode:Stop
│   └── ...
│
├── ProfileEditor
├── Events
├── Samples
└── ValueInput
```

Idle, Running, Monitor... are resulting semantic presentations depending on App state

## Accepted actions
### Main
- Brief / Idle: Start, Edit, Settings, Events
- Brief / Running: Next, Events, Stop, Edit?, Settings?
- Brief / Stopped: Reset
- Brief / Paused: Stop, Reset
- Brief / Finished: Reset, Events?
- Brief / Error: Reset
- Brief / Auto: Stop
- Detailed / Running: Prev, Stop, Events, Edit?, Settings?
### Profile Selection
- Start: Select(n), Prev, Next, Cancel?
- Edit:  Select(n), Prev, Next, Cancel?
### Edit
- Select(n), SetValue(val), ToggleOutput(n), Next, Prev, Confirm, Cancel
## Question
- Confirm, Cancel
## Settings
- PID: Select(n), Next, Prev, Confirm, Cancel
- Other: Select(n), Next, Prev, Confirm, Cancel
## Events
- Next, Prev, Cancel

## Data
### Main
- Brief / Idle: Temperature 
- Brief / Running: Profile, Step, Temperature, Power, Outputs
- Brief / Auto: Temperature, Step
- Brief / Stopped: Temperature, Profile, Step
- Brief / Waiting: Profile, Outputs
- Brief / Finished: Temperature
- Brief / Error: Temperature
- Brief / Auto: Temperature, Elapsed time, Step? 
- Detailed / Running: Profile, Step, Temperature, Power, Step elapsed time, Profile elapsed time, Step type, Outputs
### Profile Selection
- Start: Profile slots
- Edit:  Profile slots
### Edit
- Step, Duration, Setpoint, Outputs
## Question
- Title?
## Settings
- PID: Max temperature
- Other: Kp, Ki, Kd 
## Events
- Event
