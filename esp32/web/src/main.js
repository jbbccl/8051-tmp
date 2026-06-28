const BASE = document.location.origin;
const STREAM = BASE + ':81';

// ── sliders ──
const fill = el => {
  const v = +el.value, min = +el.min, max = +el.max;
  const pct = ((v - min) / (max - min)) * 100;
  el.style.setProperty('--pct', pct + '%');
};
document.querySelectorAll('input[type=range]').forEach(el => {
  fill(el);
  el.oninput = () => {
    fetch(`${BASE}/control?var=${el.id}&val=${el.value}`);
    const s = document.getElementById(el.id + '-val');
    if (s) s.textContent = el.value;
    fill(el);
  };
});

// ── toggles (render from list) ──
const TOGGLES = [
  ['awb', 'AWB'], ['awb_gain', 'AWB增益'], ['aec', 'AEC'], ['aec2', 'AEC2'],
  ['agc', 'AGC'], ['bpc', 'BPC'], ['wpc', 'WPC'], ['raw_gma', 'RawGMA'],
  ['lenc', '镜头校正'], ['hmirror', '水平'], ['vflip', '垂直'], ['dcw', 'DCW'],
  ['colorbar', '色条'],
];

const $t = document.getElementById('toggles');
TOGGLES.forEach(([id, label]) => {
  const btn = document.createElement('button');
  btn.id = id;
  btn.className = 'toggle-btn toggle-off';
  btn.textContent = label + ' OFF';
  btn.onclick = () => {
    const on = btn.textContent.includes('OFF');
    fetch(`${BASE}/control?var=${id}&val=${on ? 1 : 0}`);
    btn.textContent = label + (on ? ' ON' : ' OFF');
    btn.className = on
      ? 'toggle-btn toggle-on'
      : 'toggle-btn toggle-off';
  };
  $t.appendChild(btn);
});

// ── init: fetch status ──
fetch(BASE + '/status').then(r => r.json()).then(s => {
  document.querySelectorAll('input[type=range]').forEach(el => {
    if (s[el.id] != null) el.value = s[el.id];
    fill(el);
    const v = document.getElementById(el.id + '-val');
    if (v) v.textContent = s[el.id];
  });

  TOGGLES.forEach(([id, label]) => {
    const btn = document.getElementById(id);
    if (!btn || s[id] == null) return;
    const on = s[id] === 1 || s[id] === true;
    btn.textContent = label + (on ? ' ON' : ' OFF');
    btn.className = on
      ? 'toggle-btn toggle-on'
      : 'toggle-btn toggle-off';
  });
});

// ── select ──
document.getElementById('framesize').onchange = function () {
  fetch(`${BASE}/control?var=framesize&val=${this.value}`);
};

// ── stream / still ──
const $stream = document.getElementById('stream');
const $container = document.getElementById('stream-container');
const $placeholder = document.getElementById('stream-placeholder');
let streaming = false;

document.getElementById('toggle-stream').onclick = () => {
  streaming = !streaming;
  if (streaming) {
    $stream.src = STREAM + '/stream';
    $container.classList.remove('hidden');
    $placeholder.classList.add('hidden');
  } else {
    $stream.src = '';
    $container.classList.add('hidden');
    $placeholder.classList.remove('hidden');
  }
};

document.getElementById('close-stream').onclick = () => {
  streaming = false;
  $stream.src = '';
  $container.classList.add('hidden');
  $placeholder.classList.remove('hidden');
};

document.getElementById('get-still').onclick = () => {
  streaming = false;
  $stream.src = `${BASE}/capture?_cb=${Date.now()}`;
  $container.classList.remove('hidden');
  $placeholder.classList.add('hidden');
};

// ── theme toggle ──
document.getElementById('theme-btn').onclick = () => {
  document.documentElement.classList.toggle('light');
};
