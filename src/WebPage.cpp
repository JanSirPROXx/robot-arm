#include "WebPage.h"

namespace robotarm
{
  String makeControlPage()
  {
    String page = F(
        R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0;
      padding: 1rem;
      font-family: system-ui, sans-serif;
      font-size: 14px;
      color: #111;
      background: #fff;
    }
    .wrap { max-width: 40rem; margin: 0 auto; }
    .row {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      margin-bottom: 0.5rem;
    }
    .row label { min-width: 4rem; }
    input[type="range"] { flex: 1; }
    input[type="number"] {
      width: 4rem;
      padding: 0.25rem;
      font: inherit;
      border: 1px solid #999;
    }
    .servo {
      border: 1px solid #ccc;
      padding: 0.75rem;
      margin-bottom: 0.75rem;
    }
    .servo h2 {
      margin: 0 0 0.5rem;
      font-size: 1rem;
      font-weight: 600;
    }
    .btns {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 0.5rem;
      margin-bottom: 0.5rem;
    }
    button {
      padding: 0.5rem;
      font: inherit;
      border: 1px solid #999;
      background: #f5f5f5;
      cursor: pointer;
      touch-action: manipulation;
      user-select: none;
      -webkit-user-select: none;
    }
    button.active, button:active { background: #ddd; }
  </style>
</head>
<body>
  <main class="wrap">
    <div class="row">
      <label for="speed">Speed</label>
      <input type="range" id="speed" min="10" max="180" step="1" value="60">
      <span><span id="speedVal">60</span>&deg;/s</span>
    </div>
    <div id="servoGrid"></div>
  </main>
  <script>
    const gridEl = document.getElementById("servoGrid");
    const speedEl = document.getElementById("speed");
    const speedValEl = document.getElementById("speedVal");

    const cards = new Map();
    let speedInteracting = false;

    function clamp(value, min, max) {
      return Math.min(max, Math.max(min, value));
    }

    async function postCommand(url) {
      try {
        const response = await fetch(url);
        if (!response.ok) throw new Error("Request failed: " + response.status);
        return await response.json();
      } catch (err) {
        console.error(err);
        return null;
      }
    }

    function attachHoldButton(btn, servoId, direction) {
      let pressed = false;
      const start = (event) => {
        event.preventDefault();
        if (pressed) return;
        pressed = true;
        btn.classList.add("active");
        try { btn.setPointerCapture(event.pointerId); } catch (_) {}
        postCommand(`/move?servo=${servoId}&dir=${direction}`);
      };
      const stop = (event) => {
        if (!pressed) return;
        pressed = false;
        btn.classList.remove("active");
        if (event && event.pointerId !== undefined) {
          try { btn.releasePointerCapture(event.pointerId); } catch (_) {}
        }
        postCommand(`/move?servo=${servoId}&dir=0`);
      };
      btn.addEventListener("pointerdown", start);
      btn.addEventListener("pointerup", stop);
      btn.addEventListener("pointercancel", stop);
      btn.addEventListener("pointerleave", stop);
      btn.addEventListener("contextmenu", (e) => e.preventDefault());
    }

    function buildCard(servo) {
      const card = document.createElement("div");
      card.className = "servo";

      const title = document.createElement("h2");
      title.textContent = servo.name;

      const btns = document.createElement("div");
      btns.className = "btns";
      const backBtn = document.createElement("button");
      backBtn.type = "button";
      backBtn.textContent = "-";
      const fwdBtn = document.createElement("button");
      fwdBtn.type = "button";
      fwdBtn.textContent = "+";
      btns.append(backBtn, fwdBtn);

      const row = document.createElement("div");
      row.className = "row";
      const slider = document.createElement("input");
      slider.type = "range";
      slider.min = servo.minAngle;
      slider.max = servo.maxAngle;
      slider.value = servo.target;
      const number = document.createElement("input");
      number.type = "number";
      number.min = servo.minAngle;
      number.max = servo.maxAngle;
      number.value = servo.target;
      row.append(slider, number);

      card.append(title, btns, row);

      attachHoldButton(backBtn, servo.id, -1);
      attachHoldButton(fwdBtn, servo.id, 1);

      let sliderInteracting = false;
      slider.addEventListener("pointerdown", () => { sliderInteracting = true; });
      slider.addEventListener("input", () => {
        number.value = clamp(Number(slider.value), servo.minAngle, servo.maxAngle);
      });
      const commit = async () => {
        sliderInteracting = false;
        const angle = clamp(Number(slider.value), servo.minAngle, servo.maxAngle);
        slider.value = angle;
        number.value = angle;
        await postCommand(`/set?servo=${servo.id}&angle=${angle}`);
      };
      slider.addEventListener("pointerup", commit);
      slider.addEventListener("pointercancel", () => { sliderInteracting = false; });
      slider.addEventListener("change", commit);

      let numberInteracting = false;
      number.addEventListener("focus", () => { numberInteracting = true; });
      number.addEventListener("blur", () => { numberInteracting = false; });
      number.addEventListener("change", async () => {
        const angle = clamp(Number(number.value), servo.minAngle, servo.maxAngle);
        number.value = angle;
        slider.value = angle;
        await postCommand(`/set?servo=${servo.id}&angle=${angle}`);
        numberInteracting = false;
      });

      return {
        card,
        slider,
        number,
        isSliderActive: () => sliderInteracting,
        isNumberActive: () => numberInteracting,
      };
    }

    function applyStatus(data) {
      if (!speedInteracting && typeof data.speed === "number") {
        const rounded = Math.round(data.speed);
        speedEl.value = rounded;
        speedValEl.textContent = rounded;
      }
      data.servos.forEach((servo) => {
        let entry = cards.get(servo.id);
        if (!entry) {
          entry = buildCard(servo);
          cards.set(servo.id, entry);
          gridEl.appendChild(entry.card);
        }
        if (!entry.isSliderActive()) entry.slider.value = servo.target;
        if (!entry.isNumberActive()) entry.number.value = servo.target;
      });
    }

    async function refreshStatus() {
      const data = await postCommand("/status");
      if (data) applyStatus(data);
    }

    speedEl.addEventListener("pointerdown", () => { speedInteracting = true; });
    speedEl.addEventListener("input", () => {
      speedValEl.textContent = speedEl.value;
    });
    const commitSpeed = async () => {
      await postCommand(`/speed?value=${speedEl.value}`);
      speedInteracting = false;
    };
    speedEl.addEventListener("change", commitSpeed);
    speedEl.addEventListener("pointerup", commitSpeed);
    speedEl.addEventListener("pointercancel", () => { speedInteracting = false; });

    refreshStatus();
    setInterval(() => {
      if (document.visibilityState === "visible") refreshStatus();
    }, 200);
  </script>
</body>
</html>)HTML");
    return page;
  }
}
