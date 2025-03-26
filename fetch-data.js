const puppeteer = require('puppeteer-extra');
const StealthPlugin = require('puppeteer-extra-plugin-stealth');
const chromium = require("@sparticuz/chromium");
const express = require("express");
const { ChartJSNodeCanvas } = require("chartjs-node-canvas");
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// Configurações do gráfico
const width = 1200;
const height = 900;

const chartJSNodeCanvas = new ChartJSNodeCanvas({ width, height });

puppeteer.use(StealthPlugin());

async function main(coin) {
    const IS_LOCAL = true;
    let path = await chromium.executablePath()
    const browser = await puppeteer.launch({
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
    await new Promise(resolve => setTimeout(resolve, 2000));
    await page.setUserAgent('Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36');


    // Preencha o formulário
    await page.select('select.ip_currency_from', 'USD');
    await new Promise(resolve => setTimeout(resolve, 1000));

    // Seleciona "Real Brasileiro - BRL" no campo "Currency To"
    await page.select('select.ip_currency_to', coin);
    await new Promise(resolve => setTimeout(resolve, 1000));

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
        type: "line",
        data: {
            labels: data.map(x => x[0]),
            datasets: [{
                    label: "Cotacao",
                    data: data.map(x => parseFloat(x[1].replace(",", "."))),
                    // backgroundColor: "rgba(233, 68, 76, 0.83)",
                    borderColor: "rgb(39, 176, 194)",
                    borderWidth: 2,
                    fill: false
            }],
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    };
    return await chartJSNodeCanvas.renderToBuffer(configuration);
}

app.get("/grafico", async (req, res) => {
    let coin = ["BRL", "JPY", "EUR"];
    let chartPaths = [];

    for (let i = 0; i < coin.length; i++) {
        const data = await main(coin[i]);
        const image = await generateChart(data);
        const filePath = path.join(__dirname, `chart${coin[i]}.png`);

        // Salvar a imagem no servidor
        fs.writeFileSync(filePath, image);
        chartPaths.push(`/images/chart${coin[i]}.png`);
        console.log(`Imagem pronta: ${coin[i]}`);
    }

    // Enviar links para as imagens geradas
    let responseHTML = '<h1>Gráficos gerados</h1>';
    chartPaths.forEach((filePath) => {
        responseHTML += `<a href="${filePath}" target="_blank">Ver gráfico</a><br>`;
    });

    res.send(responseHTML);
});

// Rota para exibir as imagens geradas
app.use('/images', express.static(path.join(__dirname)));

app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}/grafico`);
});



