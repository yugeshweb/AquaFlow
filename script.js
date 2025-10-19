import { initializeApp } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-app.js";
import { getDatabase, ref, onValue, set, get } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-database.js";

// Replace these values with your own Firebase credentials
const firebaseConfig = {
  apiKey: "YOUR_API_KEY_HERE",
  authDomain: "YOUR_PROJECT_ID.firebaseapp.com",
  databaseURL: "https://YOUR_PROJECT_ID-default-rtdb.YOUR_REGION.firebasedatabase.app",
  projectId: "YOUR_PROJECT_ID",
  storageBucket: "YOUR_PROJECT_ID.appspot.com",
  messagingSenderId: "XXXXXX",
  appId: "1:XXXX:web:XXXX"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const flow1Ref = ref(db, "flow1");
const flow2Ref = ref(db, "flow2");
const pumpRef = ref(db, "pump");
const dataRef = ref(db, "data");

let totalLiters = 0, totalPrice = 0, lastF1 = 0, lastF2 = 0, lastFlow1 = 0, lastFlow2 = 0;

// Load permanent data once
async function loadData() {
  const snap = await get(dataRef);
  if (snap.exists()) {
    const d = snap.val();
    totalLiters = d.totalLiters || 0;
    totalPrice = d.totalPrice || 0;
    lastFlow1 = d.lastFlow1 || 0;
    lastFlow2 = d.lastFlow2 || 0;
    updateUsageDisplay();
  }
}
loadData();

// Save permanent data
function saveData() {
  set(dataRef, { totalLiters, totalPrice, lastFlow1, lastFlow2 });
}

const onBtn = document.getElementById("onBtn");
const offBtn = document.getElementById("offBtn");

window.setPump = (state) => {
  set(pumpRef, state);
  onBtn.classList.toggle('active', state === 'ON');
  offBtn.classList.toggle('active', state === 'OFF');
};

function updateUsageDisplay() {
  document.getElementById("liveLiters").innerText = totalLiters.toFixed(2);
  document.getElementById("livePrice").innerText = totalPrice.toFixed(2);
}

// Listen for flow updates
onValue(flow1Ref, snap => {
  const f1 = snap.val() || 0;
  document.getElementById("flow1").innerText = (f1 * 1000 / 60).toFixed(1);
  document.getElementById("s1Status").classList.toggle('active', f1 > 0);
  lastF1 = f1;
  checkLeak();
  if (f1 > lastFlow1) {
    lastFlow1 = f1;
    saveData();
  }
});

onValue(flow2Ref, snap => {
  const f2 = snap.val() || 0;
  document.getElementById("flow2").innerText = (f2 * 1000 / 60).toFixed(1);
  document.getElementById("s2Status").classList.toggle('active', f2 > 0);
  lastF2 = f2;
  checkLeak();

  if (f2 > lastFlow2) {
    const diff = f2 - lastFlow2;
    totalLiters += diff;
    totalPrice = totalLiters * 0.3; // Adjust pricing logic as needed
    lastFlow2 = f2;
    saveData();
    updateUsageDisplay();
  }
});

document.getElementById("resetBtn").addEventListener('click', () => {
  totalLiters = 0;
  totalPrice = 0;
  lastFlow1 = 0;
  lastFlow2 = 0;
  saveData();
  updateUsageDisplay();
});

function checkLeak() {
  const leakDot = document.getElementById("leakStatus");
  const isNormal = Math.abs(lastF1 - lastF2) < 0.05;
  leakDot.classList.toggle('active', isNormal);
  leakDot.style.background = isNormal ? '#51cf66' : '#ff6b6b';
}
