param([string]$json, [string]$outdir, [int]$rate = 0)
Add-Type -AssemblyName System.Speech
$items = Get-Content -Raw -Encoding UTF8 $json | ConvertFrom-Json
$i = 0
foreach ($it in $items) {
  $s = New-Object System.Speech.Synthesis.SpeechSynthesizer
  $s.SelectVoice("Microsoft Hedda Desktop")
  $s.Rate = $rate
  $fmt = New-Object System.Speech.AudioFormat.SpeechAudioFormatInfo(24000, [System.Speech.AudioFormat.AudioBitsPerSample]::Sixteen, [System.Speech.AudioFormat.AudioChannel]::Mono)
  $s.SetOutputToWaveFile((Join-Path $outdir ("sprache$i.wav")), $fmt)
  $s.Speak($it.text)
  $s.Dispose()
  $i++
}
