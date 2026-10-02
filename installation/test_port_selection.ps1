# Charge uniquement la fonction de sélection : aucun accès matériel ni flash.
$ErrorActionPreference = 'Stop'
$tokens = $null
$parseErrors = $null
$source = [System.IO.File]::ReadAllText((Join-Path $PSScriptRoot 'installer.ps1'), [System.Text.Encoding]::UTF8)
$ast = [System.Management.Automation.Language.Parser]::ParseInput($source, [ref]$tokens, [ref]$parseErrors)
if ($parseErrors.Count -gt 0) { throw 'Erreur de syntaxe dans installer.ps1' }
$functionAst = $ast.Find({ param($node)
    $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq 'Select-PortEcran'
}, $true)
. ([scriptblock]::Create($functionAst.Extent.Text))

function Read-Host {
    param($Prompt)
    $script:prompts++
    return $script:answer
}
function Test-Selection {
    param($Label, $Ports, $Names, $Answer, $Expected, $PromptCount)
    $script:prompts = 0
    $script:answer = $Answer
    $actual = Select-PortEcran -Ports $Ports -DeviceNames $Names
    if ($actual -cne $Expected -or $script:prompts -ne $PromptCount) { throw "Echec : $Label" }
    Write-Host "OK : $Label"
}
Test-Selection 'CH340 unique avec autre port' @('COM3', 'COM12') @('USB-SERIAL CH340 (COM12)', 'USB Serial (COM3)') '' 'COM12' 0
Test-Selection 'Plusieurs CH340' @('COM3', 'COM12') @('USB-SERIAL CH340 (COM12)', 'CH340 (COM3)') 'com3' 'COM3' 1
Test-Selection 'Aucun CH340' @('COM3') @('USB Serial (COM3)') 'COM3' 'COM3' 1
Test-Selection 'Enumeration indisponible' @('COM3') @() 'COM3' 'COM3' 1
Test-Selection 'CH340 deconnecte ignore' @('COM3') @('CH340 (COM12)') 'COM3' 'COM3' 1
Test-Selection 'Doublon du meme port' @('COM12') @('CH340 (COM12)', 'CH340 (COM12)') '' 'COM12' 0
$script:answer = 'COM99'
$rejected = $false
try { Select-PortEcran -Ports @('COM3') -DeviceNames @() } catch { $rejected = $true }
if (-not $rejected) { throw 'Port absent accepte' }
Write-Host 'OK : port absent refuse'
