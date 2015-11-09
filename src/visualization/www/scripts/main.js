// For any third party dependencies, like jQuery, place them in the lib folder.

// Configure loading modules from the lib directory,
// except for 'app' ones, which are in a sibling
// directory.
//var isDebugging = true; // false;

require.config({
    //For development purposes, We force RequireJS to bypass the cache by appending a timestamp.
    //Delete this for release version.
    urlArgs: /*isDebugging*/ true ? "bust=" + (new Date()).getTime() : "",
    baseUrl: 'scripts',
    paths: {
        jquery: 'lib/jquery-2.1.3',
        d3: 'lib/d3.v3',
        dataToExcel : 'lib/dataToExcel',
        tipsy: 'lib/jquery.tipsy',
        xlsx: 'lib/xlsx.core.min',
        config: '../config/config',
        core: 'main/gvis/gvis.core',
        language: 'main/language',
        bootstrap: 'bootstrap/js/bootstrap',
        multiselect:'bootstrap/js/bootstrap-multiselect',
        datetime: 'bootstrap/js/bootstrap-datetimepicker',
        datetime_zhCN: 'bootstrap/js/locales/bootstrap-datetimepicker.zh-CN',
        colorpicker: 'bootstrap/js/bootstrap-colorpicker',
        slider: 'bootstrap/js/bootstrap-slider',
        utils: 'main/gvis/gvis.utils',
        data: 'main/gvis/gvis.data',
        settings: 'main/gvis/gvis.settings',
        donuts:'lib/Donut3D',
        ui:'main/ui',
        gvis: 'main/gvis/gvis'
    },
    shim: {
        ui: {
            deps: ['core', 'multiselect', 'datetime_zhCN', 'colorpicker', 'slider']
        },
        data: {
            deps: ['utils'],
        },
        settings: {
            deps: ['utils']
        },
        core: {
            deps: ['config', 'tipsy', 'donuts', 'd3', 'language', 'data', 'utils', 'settings'],
        },
        utils: {
            deps: ['gvis']
        },

        bootstrap:  {
            deps: ['jquery'],
        },
        multiselect: {
            deps: ['bootstrap'],
        },
        datetime:  {
            deps: ['bootstrap'],
        },
        datetime_zhCN: {
            deps: ['datetime'],
        },
        colorpicker:  {
            deps: ['bootstrap'],
        },
        slider:  {
            deps: ['bootstrap'],
        },
        language: {
            deps: ['jquery']
        },
        tipsy: {
            deps: ['jquery'],
        },
        donuts : {
            deps : ['d3']
        }
    }
});

// Start loading the main app file. Put all of
// your application logic in there.
require(['ui']);
