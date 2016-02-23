/**
 * Created by chenzhuwei on 2/18/16.
 */
angular.module('jm.i18next').config(function ($i18nextProvider) {

    'use strict';

    $i18nextProvider.options = {
        lng: 'zh', // If not given, locales will detect the browser language.
        fallbackLng: 'zh', // Default is dev
        useCookie: false,
        useLocalStorage: false,
        resGetPath: '../locales/__lng__/__ns__.json'
    };

});
