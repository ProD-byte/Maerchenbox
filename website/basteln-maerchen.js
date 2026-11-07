// Baut die "Maerchenbox" in den Rohinhalt von daveslab.de/basteln ein
// (eigene Projektkarte oben und eigener Abschnitt ganz unten, hinter der Wetterstation).
// Gleiches Prinzip wie basteln-wetter.js im Wetterstation-Repo: jede Ersetzung muss genau einmal passen,
// sonst wird nichts gespeichert. Firmware, Flasher und Bilder liegen auf GitHub Pages
// (https://prod-byte.github.io/Maerchenbox/) und werden dort bei jedem Hochladen neu gebaut -
// auf daveslab.de muss man danach nichts mehr aendern.
//
// SO BENUTZEN (selbst, in Chrome):
//   1. Bei https://daveslab.de/wp-admin/ anmelden
//   2. F12 -> Reiter "Console" -> diese ganze Datei hineinkopieren -> Enter
//      (fragt Chrome nach "allow pasting": einmal  allow pasting  eintippen, Enter, nochmal einfuegen)
//   3. Es kommt eine Rueckfrage "Speichern?" -> OK. Danach https://daveslab.de/basteln/ neu laden.
//   WordPress hebt die alte Fassung als Revision auf (Seite bearbeiten -> Revisionen).
function maerchenEinbauen(raw) {
  const fehler = [];
  let out = raw;
  function ersetze(name, such, neu) {
    const treffer = typeof such === 'string' ? out.split(such).length - 1 : (out.match(new RegExp(such.source, 'g')) || []).length;
    if (treffer !== 1) { fehler.push(name + ': ' + treffer + ' Treffer'); return; }
    out = out.replace(such, typeof neu === 'function' ? neu : () => neu);
  }
  if (out.includes('id="maerchen"')) return {out: raw, fehler: ['Maerchenbox ist schon eingebaut']};

  // --- Projektkarte oben (hinter der Wetterstation) ---
  ersetze('karte', /<a class="dl-karte" href="#wetter"[\s\S]*?<\/a>/, m => m + '\n        ' + MAERCHEN_KARTE);
  ersetze('anzahl de', '</b>3 Projekte', '</b>4 Projekte');
  ersetze('anzahl en', '<b>MAKING</b>3 projects', '<b>MAKING</b>4 projects');

  // --- Abschnitt ganz unten, vor dem Fuss ---
  ersetze('abschnitt', '<footer class="dl-foot">', MAERCHEN_ABSCHNITT + '\n\n  <footer class="dl-foot">');

  // --- Karte -> Abschnitt (die Karten oeffnen immer nur ein Projekt) ---
  ersetze('proj', "wetter:['wetter']};", "wetter:['wetter'],maerchen:['maerchen']};");

  // --- Uebersetzungen (hinter den Wetterstation-Texten) ---
  ersetze('en maerchen', "wet_web_btn:'Open settings →',", "wet_web_btn:'Open settings →',\n" + MAERCHEN_EN + ',');

  // --- Build-Nummer anzeigen ---
  ersetze('var build', "var sprache='de',build=null,buildCam=null,buildXiao=null,buildWetter=null;",
    "var sprache='de',build=null,buildCam=null,buildXiao=null,buildWetter=null,buildMaerchen=null;");
  ersetze('setze ver', "if(vw&&buildWetter!==null)vw.textContent=(W.ver||DE.ver).replace('{0}',buildWetter);",
    "if(vw&&buildWetter!==null)vw.textContent=(W.ver||DE.ver).replace('{0}',buildWetter);\n" +
    "  var vm=document.getElementById('dl-ver-maerchen');if(vm&&buildMaerchen!==null)vm.textContent=(W.ver||DE.ver).replace('{0}',buildMaerchen);\n" +
    MAERCHEN_BILDWECHSEL);
  ersetze('fetch maerchen', "buildWetter=j.build;setze(sprache);}).catch(function(){});",
    "buildWetter=j.build;setze(sprache);}).catch(function(){});\n" + MAERCHEN_SKRIPT);

  return {out: fehler.length ? raw : out, fehler};
}

// Nachruesten einer schon eingebauten Maerchenbox: Bilder wechseln mit der Sprache (bilder/en/ auf Englisch)
function maerchenNachruesten(raw) {
  const fehler = [];
  let out = raw;
  if (out.includes('data-mae=')) return {out: raw, fehler: ['Bildwechsel ist schon eingebaut']};
  const bild = /<img src="https:\/\/prod-byte\.github\.io\/Maerchenbox\/bilder\/(\w+)\.png"/g;
  const n = (out.match(bild) || []).length;
  if (n !== 9) fehler.push('bilder: ' + n + ' Treffer (erwartet 9)');
  out = out.replace(bild, '<img data-mae="$1" src="https://prod-byte.github.io/Maerchenbox/bilder/$1.png"');
  const vm = "var vm=document.getElementById('dl-ver-maerchen');if(vm&&buildMaerchen!==null)vm.textContent=(W.ver||DE.ver).replace('{0}',buildMaerchen);";
  const t = out.split(vm).length - 1;
  if (t !== 1) fehler.push('setze: ' + t + ' Treffer');
  out = out.replace(vm, () => vm + '\n' + MAERCHEN_BILDWECHSEL);
  return {out: fehler.length ? raw : out, fehler};
}

var MAERCHEN_SEITE = 'https://prod-byte.github.io/Maerchenbox';
// in setze(l): Bilder der Maerchenbox passend zur Sprache (Deutsch: bilder/, sonst bilder/en/)
var MAERCHEN_BILDWECHSEL = "  root.querySelectorAll('img[data-mae]').forEach(function(i){i.src='" + MAERCHEN_SEITE +
  "/bilder/'+(l==='de'?'':'en/')+i.getAttribute('data-mae')+'.png';});";
var MAERCHEN_GH = '<a href="https://github.com/ProD-byte/Maerchenbox" target="_blank" rel="noopener">GitHub</a>';

var MAERCHEN_KARTE = `<a class="dl-karte" href="#maerchen" data-p="maerchen" aria-expanded="false"><span class="ic">📖</span><h3>Märchenbox</h3><p data-t="k_maerchen">41 Märchen als Spieluhr-Filme – live gezeichnet und vertont.</p><small>Waveshare AMOLED 1.75C · rund</small><span class="auf-zu" data-t="k_auf">Öffnen</span></a>`;

// Flasher-Paar (Update / Neuinstallation) im Stil der anderen Projekte
function maerchenFlasher(ordner, titelKey, titel, boardKey, board) {
  return `      <h3 class="rv" style="margin:26px 0 4px" data-t="${titelKey}">${titel}</h3>
      <p class="dl-sub rv" style="margin:0 0 12px" data-t="${boardKey}">${board}</p>
      <div class="dl-flash">
        <div class="dl-opt rv">
          <h3 data-t="upd_h">🔄 Update</h3>
          <p data-t="mae_upd_p">Neue Firmware aufspielen – <b>WLAN und Einstellungen bleiben erhalten.</b> Bei „Erase device?“ den Haken <b>nicht</b> setzen.</p>
          <esp-web-install-button manifest="${MAERCHEN_SEITE}/${ordner}/manifest-update.json">
            <button slot="activate" data-t="upd_btn">⚡ Update installieren</button>
            <span slot="unsupported" data-t="unsup"><b>Dieser Browser kann das leider nicht.</b> Bitte Chrome oder Edge am Computer verwenden.</span>
            <span slot="not-allowed" data-t="notall"><b>Nur über https möglich.</b></span>
          </esp-web-install-button>
        </div>
        <div class="dl-opt rot rv">
          <h3 data-t="neu_h">🧹 Neuinstallation</h3>
          <p data-t="mae_neu_p">Speicher komplett löschen und neu aufspielen – für <b>neue Boards</b> oder wenn gar nichts mehr geht. <b>WLAN und Einstellungen sind danach weg.</b></p>
          <esp-web-install-button manifest="${MAERCHEN_SEITE}/${ordner}/manifest-neu.json">
            <button slot="activate" data-t="neu_btn">🗑️ Löschen &amp; neu installieren</button>
            <span slot="unsupported" data-t="unsup"><b>Dieser Browser kann das leider nicht.</b> Bitte Chrome oder Edge am Computer verwenden.</span>
            <span slot="not-allowed" data-t="notall"><b>Nur über https möglich.</b></span>
          </esp-web-install-button>
        </div>
      </div>`;
}

var MAERCHEN_BILDER = ['buch', 'schneiderlein', 'rotkaeppchen', 'bremen', 'rapunzel', 'sterntaler', 'start', 'gutenacht']
  .map(n => `<img data-mae="${n}" src="${MAERCHEN_SEITE}/bilder/${n}.png" alt="" loading="lazy" style="width:100%;height:auto;display:block;filter:drop-shadow(0 8px 18px rgba(0,0,0,.45))">`).join('');

var MAERCHEN_ABSCHNITT = `  <section class="dl-sec" id="maerchen">
    <div class="dl-wrap">
      <h2 class="dl-h2 rv" data-t="mae_h2">Märchenbox <span class="dl-grad">installieren</span></h2>
      <p class="dl-sub rv"><span data-t="mae_sub">41 Märchen als Spieluhr-Filme, die die Box live zeichnet und vertont – mit Märchenbuch, Sprechblasen zum Mitlesen und Einstellungen für Eltern. Für das runde <b>Waveshare ESP32-S3-Touch-AMOLED-1.75C</b> – die Märchen füllen den ganzen Kreis, bedient wird per Touch.</span> <span id="dl-ver-maerchen"></span></p>
      <div class="rv" style="display:flex;justify-content:center;margin:4px 0 18px"><img data-mae="geraet" src="${MAERCHEN_SEITE}/bilder/geraet.png" alt="Runde Märchenbox" loading="lazy" style="width:min(300px,80%);height:auto;filter:drop-shadow(0 18px 34px rgba(0,0,0,.6))"></div>
      <div class="rv" style="display:grid;grid-template-columns:repeat(auto-fill,minmax(160px,1fr));gap:14px;margin:6px 0 10px">${MAERCHEN_BILDER}</div>
${maerchenFlasher('rund', 'mae_rund_h', '◯ Runde Märchenbox', 'mae_rund_b', 'Waveshare ESP32-S3-Touch-AMOLED-1.75C · 466 × 466, Touch')}
      <ol class="dl-schritte" style="margin-top:28px">
        <li class="rv" data-t="mae_s1"><b>Box anschließen.</b> Per <b>USB-C</b> mit dem Computer verbinden (Kabel mit Datenleitungen) und <b>Chrome</b> oder <b>Edge</b> öffnen.</li>
        <li class="rv" data-t="mae_s2"><b>Aufspielen.</b> „Update installieren“ oder „Löschen &amp; neu installieren“ klicken und den Anschluss wählen – meist „USB JTAG/serial debug unit“, ein Treiber ist nicht nötig.</li>
        <li class="rv" data-t="mae_s3"><b>Keine Verbindung?</b> <b>BOOT</b> gedrückt halten, kurz <b>RESET</b> drücken, BOOT loslassen – nochmal versuchen.</li>
        <li class="rv" data-t="mae_s4"><b>Bedienen.</b> Oben tippen = lauter / zurückblättern, Mitte = Start (doppelt = nächstes Märchen, halten = Buch), unten = leiser / weiterblättern.</li>
        <li class="rv" data-t="mae_s5"><b>Für Eltern.</b> <b>BOOT</b> 3 Sekunden halten – dort Höchstlautstärke, Helligkeit, Nutzungszeiten und das <b>WLAN</b> (Handy ins WLAN „Märchenbox-Setup“).</li>
      </ol>
      <div class="dl-auto rv">
        <div>
          <h3 data-t="mae_auto_h">📖 Alles auf einer Seite</h3>
          <p data-t="mae_auto_p">Flasher, Bilder und Bedienung der Märchenbox auf einer eigenen Seite. Quellcode und Anleitung: ${MAERCHEN_GH}.</p>
        </div>
        <a class="dl-btn" href="${MAERCHEN_SEITE}/" target="_blank" rel="noopener" data-t="mae_auto_btn">Seite öffnen →</a>
      </div>
    </div>
  </section>`;

var MAERCHEN_SKRIPT = `fetch('${MAERCHEN_SEITE}/version.json',{cache:'no-store'}).then(function(r){return r.json();}).then(function(j){buildMaerchen=j.build;setze(sprache);}).catch(function(){});`;

var MAERCHEN_EN = [
  `k_maerchen:'41 fairy tales as music-box films – drawn and set to music live.'`,
  `mae_h2:'Install the <span class="dl-grad">fairy tale box</span>'`,
  `mae_sub:'41 fairy tales as music-box films that the box draws and sets to music live – with a fairy tale book, speech bubbles to read along and settings for parents. For the round <b>Waveshare ESP32-S3-Touch-AMOLED-1.75C</b> – the fairy tales fill the whole circle, controlled by touch.'`,
  `mae_rund_h:'◯ Round fairy tale box'`,
  `mae_rund_b:'Waveshare ESP32-S3-Touch-AMOLED-1.75C · 466 × 466, touch'`,
  `mae_upd_p:'Install new firmware – <b>Wi-Fi and settings are kept.</b> When asked “Erase device?”, do <b>not</b> tick the box.'`,
  `mae_neu_p:'Erase everything and install from scratch – for <b>new boards</b> or when nothing works anymore. <b>Wi-Fi and settings will be gone.</b>'`,
  `mae_s1:'<b>Connect the box.</b> Plug it into the computer via <b>USB-C</b> (a cable with data lines) and open <b>Chrome</b> or <b>Edge</b>.'`,
  `mae_s2:'<b>Flash it.</b> Click “Install update” or “Erase &amp; install” and select the port – usually “USB JTAG/serial debug unit”, no driver needed.'`,
  `mae_s3:'<b>No connection?</b> Hold <b>BOOT</b>, briefly press <b>RESET</b>, release BOOT – try again.'`,
  `mae_s4:'<b>Controls.</b> Tap top = louder / page back, middle = start (double = next tale, hold = book), bottom = quieter / page on.'`,
  `mae_s5:'<b>For parents.</b> Hold <b>BOOT</b> for 3 seconds – there you set maximum volume, brightness, usage times and <b>Wi-Fi</b> (phone to the Wi-Fi “Märchenbox-Setup”).'`,
  `mae_auto_h:'📖 Everything on one page'`,
  `mae_auto_p:'Flasher, pictures and controls of the fairy tale box on a page of its own. Source code and guide: ${MAERCHEN_GH}.'`,
  `mae_auto_btn:'Open page →'`,
].join(',\n');

// ---------- Ausfuehren: Seite holen, einbauen, nach Rueckfrage speichern ----------
(async function maerchenSpeichern() {
  if (!location.hostname.endsWith('daveslab.de')) { alert('Bitte auf daveslab.de/wp-admin ausfuehren.'); return; }
  const nonce = (typeof wpApiSettings !== 'undefined' && wpApiSettings.nonce) ||
                await (await fetch('/wp-admin/admin-ajax.php?action=rest-nonce', {credentials: 'include'})).text();
  if (!nonce || nonce === '0') { alert('Nicht angemeldet - bitte erst bei WordPress anmelden.'); return; }
  const kopf = {'X-WP-Nonce': nonce, 'Content-Type': 'application/json'};
  const liste = await (await fetch('/wp-json/wp/v2/pages?slug=basteln&context=edit&_fields=id,content', {headers: kopf, credentials: 'include'})).json();
  if (!Array.isArray(liste) || !liste[0]) { alert('Seite "basteln" nicht gefunden: ' + JSON.stringify(liste).slice(0, 200)); return; }
  const seite = liste[0];
  const schonDa = seite.content.raw.includes('id="maerchen"');  // schon eingebaut: nur nachruesten
  const {out, fehler} = schonDa ? maerchenNachruesten(seite.content.raw) : maerchenEinbauen(seite.content.raw);
  if (fehler.length) { alert('Nichts geaendert:\n' + fehler.join('\n')); return; }
  if (!confirm((schonDa ? 'Maerchenbox nachruesten (Bilder je Sprache)' : 'Maerchenbox einbauen') + ' und Seite "basteln" speichern?\n(' + seite.content.raw.length + ' -> ' + out.length + ' Zeichen)')) return;
  const r = await fetch('/wp-json/wp/v2/pages/' + seite.id, {method: 'POST', headers: kopf, credentials: 'include', body: JSON.stringify({content: out})});
  alert(r.ok ? 'Gespeichert! Jetzt https://daveslab.de/basteln/ neu laden.' : 'Fehler beim Speichern: ' + r.status + ' ' + (await r.text()).slice(0, 200));
})();
