const pupe = require('puppeteer-core');
const chromium = require("@sparticuz/chromium");
// const dayjs = require("dayjs");
const express = require("express");
const { ChartJSNodeCanvas } = require("chartjs-node-canvas");

const app = express();
const port = 3000;

// Configurações do gráfico
const width = 800;
const height = 600;

const chartJSNodeCanvas = new ChartJSNodeCanvas({ width, height });

async function main() {
    const IS_LOCAL = true;
    let path = await chromium.executablePath()
    const browser = await pupe.launch({
        args: IS_LOCAL ? [
            '--no-sandbox',
            '--disable-setuid-sandbox',
            '--disable-dev-shm-usage',
            '--disable-accelerated-2d-canvas',
            '--disable-gpu',
            '--window-size=1920x1080',
            '--start-maximized',
        ] : [...chromium.args, ...[
            '--no-sandbox',
            '--disable-setuid-sandbox',
            '--disable-dev-shm-usage',
            '--disable-accelerated-2d-canvas',
            '--disable-gpu',
            '--window-size=1920x1080',
        ]],
        defaultViewport: chromium.defaultViewport,
        executablePath: IS_LOCAL
            ? "/tmp/localChromium/chrome/linux-122.0.6261.69/chrome-linux64/chrome"
            : path,
        headless: IS_LOCAL ? false : chromium.headless,
        ignoreHTTPSErrors: true,
    });

    const page = await browser.newPage();
    await page.goto('https://www.fx-rate.net/historical/');

    // Extraia os dados da tabela
    const tableData = await page.evaluate(() => {
        const rows = document.querySelectorAll('table tr');
        let data = [];

        rows.forEach(row => {
            const cols = row.querySelectorAll('td, th');
            let rowData = [];
            cols.forEach(col => {
                rowData.push(col.innerText.trim())
            });
            data.push(rowData);
        });
        return data;
    });

    let data = tableData.slice(7);
    // console.log(data);
    await browser.close();
    return data;
}
async function generateChart(data) {
    const configuration = {
        type: "bar",
        data: {
            labels: data.map(x => x[0]),
            datasets: [
                {
                    label: "Cotacao",
                    data: data.map(x => parseFloat(x[1].replace(",", "."))) ,
                    backgroundColor: "rgba(233, 68, 76, 0.83)",
                    borderColor: "rgb(194, 39, 39)",
                    borderWidth: 3,
                },
            ],
        },
    };
    return await chartJSNodeCanvas.renderToBuffer(configuration);
}

app.get("/grafico", async (req, res) => {
    const data = await main();
    const image = await generateChart(data);
    res.set("Content-Type", "image/png");
    res.send(image);
    console.log(`imagem pronta`)
});

app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}/grafico`);
});



