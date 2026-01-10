/* --- ELEMENTS --- */
    const checkbox = document.querySelector(".toggle")
    const colorBtn = document.getElementById("colorWheelBtn");
    const colorPicker = document.getElementById("colorPicker");
    const saveBtn = document.querySelector(".save");
    const colorscene = document.querySelector(".colorscene");
    const rInput = document.getElementById("rInput");
    const gInput = document.getElementById("gInput");
    const bInput = document.getElementById("bInput");
    const sceneList = document.querySelectorAll(".scene");

    function createPreset(rgb) {
        const preset = document.createElement("div");
        preset.classList.add("preset");
        preset.classList.add("color")
        preset.style.backgroundColor = rgb;
        const bg = rgb.match(/\d+/g).map(Number);
        preset.addEventListener("click", (el) => {
            setLED(
                bg[0],
                bg[1],
                bg[2],
                0, true)
        })
        colorscene.appendChild(preset)
    }

    /* --- COLOR CHANGE --- */
    function updateWeb(r, g, b) {
        rInput.value = r;
        document.querySelector(".range.r").value = r;

        gInput.value = g;
        document.querySelector(".range.g").value = g;

        bInput.value = b;
        document.querySelector(".range.b").value = b;
    }

    function updateHeader() {
        document.querySelector("header").style
            .setProperty("--shadow", getCurrentRGB())
    }

    let sliderCooldown = false;
    function sliderUpdate(value, input, instant) {
        updateHeader()
        if (sliderCooldown && !instant) return;

        if (!instant) {
            sliderCooldown = true;
            setTimeout(() => {
                sliderCooldown = false
            }, 250); // durée cooldown
        }

        switch (value) {
            case "r":
                setR(input.value);
                setScene(0);
                break;
            case "g":
                setG(input.value);
                setScene(0);
                break;
            case "b":
                setB(input.value);
                setScene(0);
                break;
        }
    }

    function getCurrentRGB() {
        const r = Number(rInput.value);
        const g = Number(gInput.value);
        const b = Number(bInput.value);
        return `rgb(${r}, ${g}, ${b})`;
    }

    /* --- REQUEST --- */
    const setUrl = "http://192.168.1.64/set?"
    const getUrl = "http://192.168.1.64/get"
    const saveUrl = "http://192.168.1.64/save?"
    const loadUrl = "http://192.168.1.64/load"
    
    function saveColor(textRgb) {
        fetch(`${saveUrl}content=${textRgb};`)
            .then(res => res.text())
            .then(console.log);
    }

    function setR(r) {
        if (!checkbox.checked) return;
        const url = `${setUrl}r=${r}`;
        fetch(url)
        .catch(err => console.error(err));
    }

    function setG(g) {
        if (!checkbox.checked) return;
        const url = `${setUrl}g=${g}`;
        fetch(url)
        .catch(err => console.error(err));
    }

    function setB(b) {
        if (!checkbox.checked) return;
        const url = `${setUrl}b=${b}`;
        fetch(url)
        .catch(err => console.error(err));
    }

    function setScene(scene) {
        if (scene != 2 && !checkbox.checked) return;
        const url = `${setUrl}scene=${scene}`;
        fetch(url)
        .catch(err => console.error(err));
    }

    let ledCooldown
    function setLED(r, g, b, scene, instant) {
        if (ledCooldown && !instant) return;

        if (!instant) {
            ledCooldown = true;
            setTimeout(() => {
                ledCooldown = false
            }, 250);
        }

        if (!checkbox.checked) return;
        const url = `${setUrl}r=${r}&g=${g}&b=${b}&scene=${scene}`;
        fetch(url)
        .catch(err => console.error(err));
    }

    async function getState() {
        try {
            const res = await fetch(getUrl);
            const text = await res.text();
            return text.split(",");
        } catch (err) {
            console.error(err);
            return "";
        }
    }

    /* --- EVENTS --- */
    sceneList.forEach((scene) => {
        scene.addEventListener("click", () => {
            setLED(rInput.value, gInput.value, bInput.value, scene.id, true)
        })
    })

    document.addEventListener("DOMContentLoaded", () => {
        fetch(loadUrl)
            .then(res => res.text())
            .then(data => {
                console.log("RAW data:", data);
                const colors = data.split(";").filter(c => c);
                console.log("Colors array:", colors);

                colors.forEach(c => {
                    createPreset(c)
                })
            })
            .catch(console.error);
    });

    saveBtn.addEventListener("click", () => {
        const rgb = getCurrentRGB();
        // if (colorExists(rgb)) {
        //     alert("Cette couleur est déjà sauvegardée !");
        //     return;
        // }
        saveColor(rgb+";")
        console.log(rgb+";")
        createPreset(rgb);
    });

    /* --- Sliders & Inputs --- */
    const rgb  = ["r", "g", "b"];
    rgb.forEach(value => {
        const input = document.getElementById(value+"Input")
        const range = document.querySelector(".range."+value)

        getState().then(currentLEDState => {
            i = rgb.indexOf(value)
            input.value = currentLEDState[i]
            range.value = currentLEDState[i]
            updateHeader();
        })

        range.addEventListener("input", () => {
            input.value = range.value;
            sliderUpdate(value, input, false)
        })

        range.addEventListener("mouseup", () => {
            input.value = range.value;
            sliderUpdate(value, input, true)
        })

        document.getElementById(value + "InputUp").addEventListener("mousedown", () => {
            input.stepUp()
            range.value = input.value 
            sliderUpdate(value, input, true);
        })

        document.getElementById(value + "InputDown").addEventListener("mousedown", () => {
            input.stepDown()
            sliderUpdate(value, input, true);
        })

        input.addEventListener("input", () => {
            if (!input.checkValidity() || input.value === "") {
                range.value = 0
                return;
            }
            range.value = input.value
            sliderUpdate(value, input, true);
        })
    })

    /* --- ColorPicker --- */
    colorBtn.addEventListener("click", () => {
        colorPicker.click();
    });

    colorPicker.addEventListener("input", (e) => {
        const color = e.target.value;

        // convert to rgb
        const r = parseInt(color.substr(1,2),16);
        const g = parseInt(color.substr(3,2),16);
        const b = parseInt(color.substr(5,2),16);

        updateWeb(r, g, b);
        setLED(r, g, b, 0, false);
    });

    /* on/off */
    checkbox.addEventListener("input", () => {
        if (!checkbox.checked) {
            setScene(2);
        }
        else {
            setLED(
                document.getElementById("rInput").value,
                document.getElementById("gInput").value,
                document.getElementById("bInput").value,
                0, true)
        }
    })

    document.querySelector(".toggle_wrapper").addEventListener("click", () => {
        checkbox.click()
    })

    /* Color Preset ==> ClickEvent */
    document.querySelectorAll(".preset.color").forEach(el => {
        if (!el.textContent.startsWith("rgb(")) {
            el.style.backgroundColor = el.textContent;
        }
        el.style.color = el.textContent;

        const bg = getComputedStyle(el).backgroundColor
            .match(/\d+/g).map(Number); // to table
        el.addEventListener("click", () => {
            updateWeb(bg[0], bg[1], bg[2])
            setLED(
                bg[0],
                bg[1],
                bg[2],
                0, true)
        })
    })