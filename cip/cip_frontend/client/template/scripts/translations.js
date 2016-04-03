/**
 * INSPINIA - Responsive Admin Theme
 *
 */
function config($translateProvider) {
    //$translateProvider.useSanitizeValueStrategy('sanitize');
    $translateProvider
        .translations('en', {
            // Define all menu elements
            CrowdManagement: 'Crowd Management',
            ViewCrowdList: 'View Crowd List',
            ViewGroupList: 'View Group List',
            CreateCrowd: 'Create Crowd',
            CreateGroup: 'Create Group',
        })
        .translations('zh', {
            // Define all menu elements
            CrowdManagement: '圈人管理',
            ViewCrowdList: '查看人群列表',
            ViewGroupList: '查看人群组合列表',
            CreateCrowd: '创建人群',
            CreateGroup: '创建人群组合',
        });

    $translateProvider.preferredLanguage('en');
}

angular
    .module('cipApp')
    .config(config)
