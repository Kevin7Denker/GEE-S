
class Record {
  constructor(ordem, co2, metano, temperatura, umidade) {
    this.ordem = ordem;
    this.co2 = co2;
    this.temp = temperatura;
    this.met = metano;
    this.umi = umidade;
  }

  static fromJson(data) {
    return new Record(
      data["co2"],
      data["co2"],
      data["metano"],
      data["temperatura"],
      data["umidade"]
    );
  }
}

class DataApi {
  constructor(url) {
    this.url = url;
  }

  async fetch() {
    var allData = [];
    var json = await fetch(this.url).then(function (response) {
      return response.json();
      console.log(response);
    });
    Object.entries(json).forEach(([id, vals]) => {
      allData.push(Record.fromJson(vals));
    });
    return allData;
  }
}

const RepositoryStatus = {
  empty: "empty",
  loaded: "loaded",
};

class RecordsRepository {
  constructor(api) {
    this.api = api;
    this.data = [];
    this.status = RepositoryStatus.empty;
  }

  async getData() {
    this.data = await this.api.fetch();
    console.log(this.data);
    this.data.sort((d1, d2) => {
      return d1.ordem - d2.ordem;
    });
    this.status = RepositoryStatus.loaded;
  }


  async getMetSamples() {
    if (this.status !== RepositoryStatus.loaded) {
      await this.getData();
    }
    var ret = [];
    this.data.forEach((val) => {
      ret.push(val.met);
    });
    return ret;
  }

  async getUmiSamples() {
    if (this.status !== RepositoryStatus.loaded) {
      await this.getData();
    }
    var ret = [];
    this.data.forEach((val) => {
      ret.push(val.umi);
    });
    return ret;
  }

  async getTemperatureSamples() {
    if (this.status !== RepositoryStatus.loaded) {
      await this.getData();
    }
    var ret = [];
    this.data.forEach((val) => {
      ret.push(val.temp);
    });
    return ret;
  }

  
  async getOrdem() {
    if (this.status !== RepositoryStatus.loaded) {
      await this.getData();
    }
    var ret = [];
    this.data.forEach((val) => {
      ret.push(val.ordem);
    });
    return ret;
  }
  
}

class DrawChart {
  constructor(elem, repository) {
    this.elem = elem;
    this.repository = repository;
    
    this.chart = new Chart(this.elem, {
      type: "line",
      data: {
        labels: [],
        datasets: [
          {
            label: "Temperature",
            data: [],
            borderColor: "rgb(0, 192, 30)",
            tension: 0.1,
          },
          {
            label: "Metano",
            data: [],
            borderColor: "rgb(255,160,122)",
            tension: 0.1,
          },
          {
            label: "Umidade",
            data: [],
            borderColor: "	rgb(0,0,139)",
            tension: 0.1,
          },
        ],
      },
      options: {
        plugins: {
            zoom: {
                pan: {
                    enabled: true,
                    mode: 'xy',
                    overScaleMode: 'y'
                },
                zoom: {
                    enabled: true,
                    mode: 'xy',
                    overScaleMode: 'y'
                }
            }
        },
        scales: {
          y: {
            beginAtZero: true,
          },
          x:{
            beginAtZero: true,
          },
        },
        maintainAspectRatio: false,
        responsive: false,
      },
    });

    
  }
  

  async update() {
    await this.repository.getData();
    let labels = await this.repository.getOrdem();
    let temperatureSamples = await this.repository.getTemperatureSamples();
    let metSamples = await this.repository.getMetSamples();
    let umiSamples = await this.repository.getUmiSamples();

    this.chart.data.labels = labels;
    this.chart.data.datasets[0].data = temperatureSamples;
    this.chart.data.datasets[1].data = metSamples;
    this.chart.data.datasets[2].data = umiSamples;

    this.chart.update();
  }
}
