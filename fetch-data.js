const puppeteer = require('puppeteer-extra');
const StealthPlugin = require('puppeteer-extra-plugin-stealth');
const chromium = require("@sparticuz/chromium");
const express = require("express");
const dayjs = require("dayjs");
const customParseFormat = require('dayjs/plugin/customParseFormat');
dayjs.extend(customParseFormat);
const { ChartJSNodeCanvas } = require("chartjs-node-canvas");
const fs = require('fs');
const path = require('path');

const app = express();
const port = 3000;

// Configurações do gráfico
const width = 1200;
const height = 900;

const chartJSNodeCanvas = new ChartJSNodeCanvas({ width, height });

function MTC (data) {
    let mediaAr = data.reduce((acc, val) => acc + parseFloat(val[1].replace(",", ".")), 0);
    console.log("total: ", mediaAr , "n de elementos:", data.length);
    mediaAr /= data.length;
    console.log("media: ", mediaAr);

    let array = []
    data.forEach(d => {
        let obj = array.find(o => o[1] == d[1]);
        if (obj) {
            obj[0]++;
        } else {
            array.push(1, d[1]);
        }
    })

    let moda = array.sort((acc, val) => val[0] - acc[0])[1];
    console.log("moda: ",moda);

    let array_mediana = data.map(x => parseFloat(x[1].replace(",", ".")));
    array_mediana.sort((acc, val) => acc - val);
    let mediana = 0;
    if (data.length % 2 == 0) {
        mediana = (array_mediana[data.length / 2] + array_mediana[data.length / 2 + 1]) / 2; 
    }else{
        mediana = array_mediana[data.length / 2 + 1];
    }
    console.log("mediana: ",mediana);
}

function Variabilty(data){
    let max = data.reduce((acc, val) => acc > parseFloat(val[1].replace(",", ".")) ? acc : parseFloat(val[1].replace(",", ".")), data[0][1]);
    let min = data.reduce((acc, val) => acc < parseFloat(val[1].replace(",", ".")) ? acc : parseFloat(val[1].replace(",", ".")), data[0][1]);
    console.log("max: ",max);
    console.log("min: ",min);
    console.log("R", max-min);


    let variabilidade = 0;
    let media = data.reduce((acc, val) => acc + parseFloat(val[1].replace(",", ".")), 0)/ data.length;
    data.forEach(d => {
        // console.log(Math.pow(media - parseFloat(d[1].replace(",", ".")), 2));
        variabilidade += Math.pow(media - parseFloat(d[1].replace(",", ".")), 2);
    })

    console.log("Variancia numerador: ",variabilidade);
    variabilidade = variabilidade / (data.length-1);
    console.log("Variancia: ",variabilidade);

    let desvioPadrao = Math.sqrt(variabilidade);
    console.log('desvio padrao: ',desvioPadrao);
    console.log('desvio padrao(%): ',desvioPadrao*100,"%");

    let coeficienteVariabilidade = desvioPadrao / media*100;
    console.log('cv: ',coeficienteVariabilidade,'%');

}

puppeteer.use(StealthPlugin());

async function main(coin) {
    const IS_LOCAL = false;
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
    await new Promise(resolve => setTimeout(resolve, 3000));
    await page.setUserAgent('Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36');


    // Preencha o formulário
    await page.select('select.ip_currency_from', 'USD');
    await new Promise(resolve => setTimeout(resolve, 2000));

    // Seleciona "Real Brasileiro - BRL" no campo "Currency To"
    await page.select('select.ip_currency_to', coin);
    await new Promise(resolve => setTimeout(resolve, 3000));

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

    data.forEach(d => {
        const data = `${d[0].slice(0,5)}-20${d[0].slice(6)}`
        d[0] = data;
    })

    data.sort((acc, val) => dayjs(acc[0], "DD-MM-YYYY").valueOf() - dayjs(val[0], "DD-MM-YYYY").valueOf());
    MTC(data);
    Variabilty(data)
    await browser.close();
    fs.writeFileSync('arquivo.txt', data.join('\n'));
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
        console.log(coin[i]);
        const data = await main(coin[i]);
        const image = await generateChart(data);
        const filePath = path.join(__dirname, `chart${coin[i]}.png`);

        // Salvar a imagem no servidor
        fs.writeFileSync(filePath, image);
        chartPaths.push(`/images/chart${coin[i]}.png`);
        console.log(`Imagem pronta: ${coin[i]}\n`);
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



