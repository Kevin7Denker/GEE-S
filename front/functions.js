class Record{
    constructor(ordem,co2,metano,temperatura){
        this.ordem = ordem;
        this.co2 = co2;
        this.temp = temperatura;
        this.met = metano;
    }

    static fromJson(data){
        return new Record(data['ordem'],data['co2'],data['metano'],data['temperatura']);
    }

}

class DataApi{

    constructor(url){
        this.url = url;
    }

    async fetch(){
        var allData = [];
        var json = await fetch(this.url).then(function(response){
            return response.json();
        });
        Object.entries(json).forEach(([id,vals])=>{
                allData.push(Record.fromJson(vals));
            });
        return allData;
    }
}


const RepositoryStatus = {
    empty:'empty',
    loaded:'loaded'
}

class RecordsRepository{

    constructor(api){
        this.api = api;
        this.data = [];
        this.status = RepositoryStatus.empty;
    }

    async getData(){
        this.data = await this.api.fetch();
        this.data.sort((d1,d2)=>{
            return d1.ordem - d2.ordem;
        });
        this.status = RepositoryStatus.loaded;

    }

    async getCo2Samples(){
        if(this.status !== RepositoryStatus.loaded){
            await this.getData();
        }
        var ret = [];
        this.data.forEach((val)=>{
            ret.push(val.co2);
        });
        return ret; 
    }

    async getTemperatureSamples(){
        if(this.status !== RepositoryStatus.loaded){
            await this.getData();
        }
        var ret = [];
        this.data.forEach((val)=>{
            ret.push(val.temp);
        });
        return ret; 
    }

    async getOrdem(){
        if(this.status !== RepositoryStatus.loaded){
            await this.getData();
        }
        var ret = [];
        this.data.forEach((val)=>{
            ret.push(val.ordem);
        });
        return ret; 
    }

}

class DrawChart{

    constructor(elem,repository){
        this.elem = elem;
        this.repository = repository;
        this.chart = new Chart(this.elem, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Co2',
                    data: [],
                    borderColor: 'rgb(75, 192, 192)',
                    tension: 0.1,
                },
                {
                    label: 'Temperature',
                    data: [],
                    borderColor: 'rgb(0, 192, 30)',
                    tension: 0.1,
                },
            ]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true
                    }
                }
            }
        });
    }

    async update(){
        await this.repository.getData();
        let labels = await this.repository.getOrdem();
        let co2Samples = await this.repository.getCo2Samples();
        let temperatureSamples = await this.repository.getTemperatureSamples();
        
        this.chart.data.labels = labels;
        this.chart.data.datasets[0].data = co2Samples;
        this.chart.data.datasets[1].data = temperatureSamples;
        
        this.chart.update();

    }


}


