'use strict';

/*
 * Defining the Package
 */
var Module = require('meanio').Module;

var I18n = new Module('i18n');

/*
 * All MEAN packages require registration
 * Dependency injection is used to define required modules
 */
I18n.register(function(app, auth, database) {

  //We enable routing. By default the Package Object is passed to the routes
  I18n.routes(app, auth, database);

  I18n.angularDependencies([ 'mean.system']);

  /**
    //Uncomment to use. Requires meanio@0.3.7 or above
    // Save settings with callback
    // Use this for saving data from administration pages
    I18n.settings({
        'someSetting': 'some value'
    }, function(err, settings) {
        //you now have the settings object
    });

    // Another save settings example this time with no callback
    // This writes over the last settings.
    I18n.settings({
        'anotherSettings': 'some value'
    });

    // Get settings. Retrieves latest saved settigns
    I18n.settings(function(err, settings) {
        //you now have the settings object
    });
    */

  return I18n;
});
