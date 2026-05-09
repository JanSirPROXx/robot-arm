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
  <title>Robotic Arm Control</title>
  <style>
    :root {
      --bg: #f4efe7;
      --panel: #fffaf3;
      --line: #d9ccb7;
      --text: #1f1b16;
      --muted: #6d6356;
      --accent: #bc5f2d;
      --accent-dark: #8f451f;
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: "Trebuchet MS", "Segoe UI", sans-serif;
      color: var(--text);
      background:
        radial-gradient(circle at top left, rgba(188, 95, 45, 0.18), transparent 24rem),
        linear-gradient(180deg, #f8f3eb 0%, var(--bg) 100%);
      min-height: 100vh;
    }
    .wrap {
      width: min(56rem, calc(100% - 2rem));
      margin: 0 auto;
      padding: 2rem 0 3rem;
    }
    .hero {
      display: grid;
      gap: 0.5rem;
      padding: 1.5rem;
      border: 1px solid rgba(31, 27, 22, 0.08);
      border-radius: 1.5rem;
      background: rgba(255, 250, 243, 0.8);
      backdrop-filter: blur(12px);
      box-shadow: 0 1rem 2rem rgba(71, 49, 31, 0.08);
    }
    h1 {
      margin: 0;
      font-size: clamp(1.8rem, 5vw, 3.4rem);
      line-height: 0.95;
      letter-spacing: 0.04em;
      text-transform: uppercase;
    }
    .sub, .status {
      margin: 0;
      color: var(--muted);
    }
    .grid {
      display: grid;
      gap: 1rem;
      margin-top: 1.5rem;
    }
    .card {
      padding: 1rem;
      border-radius: 1.25rem;
      border: 1px solid var(--line);
      background: var(--panel);
      box-shadow: 0 0.5rem 1.5rem rgba(71, 49, 31, 0.06);
    }
    .card-head {
      display: flex;
      justify-content: space-between;
      align-items: baseline;
      gap: 1rem;
      margin-bottom: 0.75rem;
    }
    .card h2 {
      margin: 0;
      font-size: 1.15rem;
    }
    .value {
      color: var(--accent-dark);
      font-weight: 700;
      min-width: 4.5rem;
      text-align: right;
    }
    .meta {
      margin: 0 0 0.75rem;
      color: var(--muted);
      font-size: 0.95rem;
    }
    .controls {
      display: grid;
      gap: 0.75rem;
      grid-template-columns: minmax(0, 1fr) 5.5rem;
      align-items: center;
    }
    input[type="range"] {
      width: 100%;
      accent-color: var(--accent);
    }
    input[type="number"] {
      width: 100%;
      padding: 0.65rem 0.75rem;
      border-radius: 0.8rem;
      border: 1px solid var(--line);
      background: #fff;
      font: inherit;
      color: inherit;
    }
    .footer {
      margin-top: 1rem;
      color: var(--muted);
      font-size: 0.92rem;
    }
  </style>
</head>
<body>
  <main class="wrap">
    <section class="hero">
      <p class="sub">On-device control panel</p>
      <h1>Robotic Arm</h1>
      <p class="status" id="network">Loading connection details...</p>
    </section>
    <section class="grid" id="servoGrid"></section>
    <p class="footer">Edit the servo names, pins, and limits in <code>src/AppConfig.cpp</code>.</p>
  </main>
  <script>
    const networkEl = document.getElementById("network");
    const gridEl = document.getElementById("servoGrid");

    function clamp(value, min, max) {
      return Math.min(max, Math.max(min, value));
    }

    async function setServo(id, angle) {
      const response = await fetch(`/set?servo=${id}&angle=${angle}`);
      if (!response.ok) {
        throw new Error("Servo update failed");
      }
    }

    function attachServoEvents(slider, number, valueLabel, servo) {
      const sync = (value) => {
        const angle = clamp(Number(value), servo.minAngle, servo.maxAngle);
        slider.value = angle;
        number.value = angle;
        valueLabel.textContent = `${angle} deg`;
        return angle;
      };

      slider.addEventListener("input", () => {
        sync(slider.value);
      });

      slider.addEventListener("change", async () => {
        const angle = sync(slider.value);
        await setServo(servo.id, angle);
      });

      number.addEventListener("change", async () => {
        const angle = sync(number.value);
        await setServo(servo.id, angle);
      });
    }

    function renderServo(servo) {
      const card = document.createElement("article");
      card.className = "card";

      const header = document.createElement("div");
      header.className = "card-head";

      const title = document.createElement("h2");
      title.textContent = servo.name;

      const valueLabel = document.createElement("div");
      valueLabel.className = "value";
      valueLabel.textContent = `${servo.angle} deg`;

      header.append(title, valueLabel);

      const meta = document.createElement("p");
      meta.className = "meta";
      meta.textContent = `GPIO ${servo.pin} - Range ${servo.minAngle} to ${servo.maxAngle} deg`;

      const controls = document.createElement("div");
      controls.className = "controls";

      const slider = document.createElement("input");
      slider.type = "range";
      slider.min = servo.minAngle;
      slider.max = servo.maxAngle;
      slider.value = servo.angle;

      const number = document.createElement("input");
      number.type = "number";
      number.min = servo.minAngle;
      number.max = servo.maxAngle;
      number.value = servo.angle;

      controls.append(slider, number);
      card.append(header, meta, controls);
      attachServoEvents(slider, number, valueLabel, servo);
      return card;
    }

    async function loadStatus() {
      const response = await fetch("/status");
      const data = await response.json();
      networkEl.textContent =
        `${data.mode} mode on ${data.network} - open http://${data.ip}`;
      gridEl.innerHTML = "";
      data.servos.forEach((servo) => gridEl.appendChild(renderServo(servo)));
    }

    loadStatus().catch((error) => {
      networkEl.textContent = error.message;
    });
  </script>
</body>
</html>)HTML");
    return page;
  }
}
