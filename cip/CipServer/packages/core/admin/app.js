'use strict';

/*
 * Defining the Package
 */
var Module = require('meanio').Module;
var Admin = new Module('admin');

/*
 * All MEAN packages require registration
 * Dependency injection is used to define required modules
 */

Admin.register(function(app, auth, database, circles) {

    var icons = 'admin/assets/img/icons/';

    //Admin.menus.add({
    //    title: 'admin settings',
    //    link: 'admin settings',
    //    roles: ['admin'],
    //    menu: 'main'
    //});
    Admin.menus.add({
        title: 'Crowd_Manager',
        link: 'crowd',
        roles: ['admin'],
        icon: icons + 'crowd.png',
        menu: 'admin'
    });
    Admin.menus.add({
        title: 'Create_Crowd',
        link: 'create crowd',
        roles: ['admin'],
        icon: icons + 'create.png',
        menu: 'admin'
    });
    Admin.menus.add({
        title: 'Group_Manager',
        link: 'group',
        roles: ['admin'],
        icon: icons + 'group.png',
        menu: 'admin'
    });
    Admin.menus.add({
        title: 'Create_Group',
        link: 'create group',
        roles: ['admin'],
        icon: icons + 'create_group.png',
        menu: 'admin'
    });
    //Admin.menus.add({
    //    title: 'BRAIN_BUILDER',
    //    link: 'brain',
    //    roles: ['admin'],
    //    icon: icons + 'modules.png',
    //    menu: 'admin'
    //});
    //Admin.menus.add({
    //    roles: ['admin'],
    //    title: 'SETTINGS',
    //    link: 'settings',
    //    icon: icons + 'settings.png',
    //    menu: 'admin'
    //});

    Admin.aggregateAsset('css', 'admin.css');
    Admin.aggregateAsset('js', '../lib/ng-clip/src/ngClip.js', {
        absolute: false,
        global: true
    });

    Admin.aggregateAsset('js', '../lib/zeroclipboard/dist/ZeroClipboard.js', {
        absolute: false,
        global: true
    });

    Admin.angularDependencies(['ngClipboard']);

    // We enable routing. By default the Package Object is passed to the routes
    Admin.routes(app, auth, database);
    return Admin;
});
