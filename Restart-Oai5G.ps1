[CmdletBinding()]
param(
    [string]$Board = 'ubuntu@10.17.98.165',
    [string]$AtPort = 'COM12',
    [string]$MbnInterface = '',
    [int]$RegistrationTimeoutSeconds = 90
)

$ErrorActionPreference = 'Stop'

if (-not $MbnInterface) {
    $MbnInterface = (Get-NetAdapter |
        Where-Object { $_.InterfaceDescription -like '*Quectel Wireless Ethernet*' } |
        Select-Object -First 1).Name
}
if (-not $MbnInterface) { throw 'Quectel MBIM network interface was not found' }

function Invoke-At {
    param(
        [Parameter(Mandatory)] [string[]]$Commands,
        [int]$DelayMilliseconds = 1500
    )

    $port = [System.IO.Ports.SerialPort]::new($AtPort, 115200, 'None', 8, 'One')
    $port.DtrEnable = $true
    $port.RtsEnable = $true
    try {
        $port.Open()
        foreach ($command in $Commands) {
            $port.Write("$command`r")
            Start-Sleep -Milliseconds $DelayMilliseconds
            $response = $port.ReadExisting()
            Write-Host "[$command] $response"
            if ($response -match '\bERROR\b') {
                throw "AT command failed: $command"
            }
        }
    }
    finally {
        if ($port.IsOpen) { $port.Close() }
        $port.Dispose()
    }
}

Write-Host '1/5 Put RM520N offline and clear its old NAS/PDU state...'
Invoke-At -Commands @('AT+CFUN=0') -DelayMilliseconds 3000

Write-Host '2/5 Restart the complete A-board core/gNB stack...'
& ssh -o BatchMode=yes -o ConnectTimeout=8 $Board `
    'sudo /home/ubuntu/openairinterface5g/ext/cn5gcorenetwork/scripts/run-a-no-v.sh restart'
if ($LASTEXITCODE -ne 0) { throw "A-board restart failed: exit $LASTEXITCODE" }

Write-Host '3/5 Restore NR5G-only/n78 and bring the modem online...'
Invoke-At -Commands @(
    'AT+QNWPREFCFG="mode_pref",NR5G',
    'AT+QNWPREFCFG="nr5g_band",78',
    'AT+CFUN=1'
) -DelayMilliseconds 3000

Write-Host '4/5 Warm-restart B210 while RM520N is actively searching...'
Start-Sleep -Seconds 8
& ssh -o BatchMode=yes -o ConnectTimeout=8 $Board `
    'sudo /home/ubuntu/openairinterface5g/scripts/run-a-no-v-gnb.sh stop; sleep 3; sudo /home/ubuntu/openairinterface5g/scripts/run-a-no-v-gnb.sh start'
if ($LASTEXITCODE -ne 0) { throw "B210 warm restart failed: exit $LASTEXITCODE" }

Write-Host '5/5 Wait for Windows MBIM registration and data connection...'
$deadline = (Get-Date).AddSeconds($RegistrationTimeoutSeconds)
$connected = $false
do {
    Start-Sleep -Seconds 3
    $state = (& netsh mbn show connection interface="$MbnInterface") -join "`n"
    if ($state -match 'Interface State\s*:\s*Connected') {
        $connected = $true
        break
    }
} while ((Get-Date) -lt $deadline)

if (-not $connected) {
    throw "UE did not reconnect within $RegistrationTimeoutSeconds seconds"
}

$configuration = Get-NetIPConfiguration -InterfaceAlias $MbnInterface
$ueAddress = ($configuration.IPv4Address |
    Where-Object { $_.IPAddress -like '12.1.1.*' } |
    Select-Object -First 1).IPAddress
if (-not $ueAddress) { throw 'Connected, but no 12.1.1.x UE address was assigned' }

Write-Host "OAI 5G connected. UE IPv4: $ueAddress"
& ping -n 4 -S $ueAddress 192.168.71.140
if ($LASTEXITCODE -ne 0) { throw 'OAI user-plane ping failed' }
