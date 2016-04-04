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
            Crowd_view : {
                NoCrowdExist: 'No crowd exist, please create one!',
                CrowdList: 'Crowd List',
                CreateNewCrowdList: 'Create New Crowd List',
                CrowdID: 'ID',
                CrowdName: 'Name',
                CrowdType: 'Type',
                AmountPeople: 'Users',
                CreateDate: 'Date',
                Status: 'Status',
                Operation: 'Operation',
                CreateInProcessing: 'Creating',
                CreateComplete: 'Create Complete',
                CreateFailed: 'Create Failed',
                CrowdDetail: 'Detail',
                CrowdDelete: 'Delete',
                CrowdDeleteComfirmMessage: 'Do you want to delete the crowd?'

            }
        })
        .translations('zh', {
            // Define all menu elements
            CrowdManagement: '圈人管理',
            ViewCrowdList: '查看人群列表',
            ViewGroupList: '查看人群组合列表',
            CreateCrowd: '创建人群',
            CreateGroup: '创建人群组合',
            Crowd_view: {
                NoCrowdExist: '您还没有创建人群,现在开始创建一个人群吧!',
                CrowdList: '人群列表',
                CreateNewCrowdList: '创建新的人群',
                CrowdID: '序号',
                CrowdName: '人群名称',
                CrowdType: '类型',
                AmountPeople: '覆盖人数',
                CreateDate: '创建时间',
                Status: '状态',
                Operation: '操作',
                CreateInProcessing: '创建中',
                CreateComplete: '创建完成',
                CreateFailed: '创建失败',
                CrowdDetail: '详情',
                CrowdDelete: '删除',
                CrowdDeleteComfirmMessage: '确定要删除这个人群吗？'
            }
        });

    $translateProvider.preferredLanguage('en');
}

angular
    .module('cipApp')
    .config(config)
