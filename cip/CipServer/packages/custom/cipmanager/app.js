'use strict';

/*
 * Defining the Package
 */
var Module = require('meanio').Module;

var Cipmanager = new Module('cipmanager');

/*
 * All MEAN packages require registration
 * Dependency injection is used to define required modules
 */
Cipmanager.register(function(app, auth, database) {

  //We enable routing. By default the Package Object is passed to the routes
  Cipmanager.routes(app, auth, database);

  //We are adding a link to the main menu for all authenticated users

  Cipmanager.aggregateAsset('css', 'cipmanager.css');

  Cipmanager.angularDependencies(['mean.system']);

  /**
    //Uncomment to use. Requires meanio@0.3.7 or above
    // Save settings with callback
    // Use this for saving data from administration pages
    Cipmanager.settings({
        'someSetting': 'some value'
    }, function(err, settings) {
        //you now have the settings object
    });

    // Another save settings example this time with no callback
    // This writes over the last settings.
    Cipmanager.settings({
        'anotherSettings': 'some value'
    });

    // Get settings. Retrieves latest saved settigns
    Cipmanager.settings(function(err, settings) {
        //you now have the settings object
    });
    */

  return Cipmanager;
});
