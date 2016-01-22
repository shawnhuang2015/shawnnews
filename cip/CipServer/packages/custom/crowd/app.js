'use strict';

/*
 * Defining the Package
 */
var Module = require('meanio').Module;

var Crowd = new Module('crowd');

/*
 * All MEAN packages require registration
 * Dependency injection is used to define required modules
 */
Crowd.register(function(app, auth, database) {

  //We enable routing. By default the Package Object is passed to the routes
  Crowd.routes(app, auth, database);

  //We are adding a link to the main menu for all authenticated users
  Crowd.menus.add({
    title: 'crowd example page',
    link: 'crowd example page',
    roles: ['authenticated'],
    menu: 'main'
  });
  
  Crowd.aggregateAsset('css', 'crowd.css');

  /**
    //Uncomment to use. Requires meanio@0.3.7 or above
    // Save settings with callback
    // Use this for saving data from administration pages
    Crowd.settings({
        'someSetting': 'some value'
    }, function(err, settings) {
        //you now have the settings object
    });

    // Another save settings example this time with no callback
    // This writes over the last settings.
    Crowd.settings({
        'anotherSettings': 'some value'
    });

    // Get settings. Retrieves latest saved settigns
    Crowd.settings(function(err, settings) {
        //you now have the settings object
    });
    */

  return Crowd;
});
