# Conda Environment Setup Guide

This project uses a conda environment to manage Python and Node.js dependencies.

## Environment Created ✅

The conda environment `multi-sensor-network` has been successfully created with:
- Python 3.10
- Node.js 24.9.0
- npm 11.6.0

## How to Use the Environment

### Option 1: Use Anaconda Prompt (Easiest)

1. Open **Anaconda Prompt** from the Start menu
2. Activate the environment:
   ```cmd
   conda activate multi-sensor-network
   ```
3. Navigate to the gateway-server directory:
   ```cmd
   cd "C:\Users\isaak\Desktop\Mechanical engineering\Jaar 3\Q2\multi-sensor-network-main\multi-sensor-network-main\gateway-server"
   ```
4. Start the server:
   ```cmd
   npm start
   ```

### Option 2: Use PowerShell with Full Paths

If you're using regular PowerShell, you can use the full paths to the conda environment's executables:

```powershell
# Navigate to gateway-server
cd "C:\Users\isaak\Desktop\Mechanical engineering\Jaar 3\Q2\multi-sensor-network-main\multi-sensor-network-main\gateway-server"

# Use npm from the conda environment
& "C:\Users\isaak\anaconda3\envs\multi-sensor-network\npm.cmd" start
```

### Option 3: Initialize Conda for PowerShell

To use `conda activate` in PowerShell:

1. Run this once:
   ```powershell
   & "C:\Users\isaak\anaconda3\Scripts\conda.exe" init powershell
   ```
2. **Close and reopen PowerShell** (or restart your terminal)
3. Then you can use:
   ```powershell
   conda activate multi-sensor-network
   npm start
   ```

## Recreating the Environment

If you need to recreate this environment on another machine or after deletion:

```cmd
conda env create -f environment.yml
```

## Environment Location

The environment is located at:
```
C:\Users\isaak\anaconda3\envs\multi-sensor-network
```

## Verifying Installation

To verify Node.js and npm are available:

```cmd
node --version    # Should show: v24.9.0
npm --version     # Should show: 11.6.0
```

## Dependencies Installed

The Node.js dependencies (express, cors) have been installed in:
```
gateway-server/node_modules
```

You can now run `npm start` to start the gateway server!

