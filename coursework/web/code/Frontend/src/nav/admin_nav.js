export default {
  items: [
    {
      name: '修改密码',
      url: '/changePassword',
      icon: 'icon-speedometer'
    },
    {
      title: true,
      name: '管理员功能',
      class: '',
      wrapper: {
        element: '',
        attributes: {}
      }
    },
    {
      name: '学生地区',
      url: '/studentMap',
      icon: 'icon-puzzle',
    },
    {
      name: '增加用户',
      url: '/addPerson',
      icon: 'icon-puzzle',
    },
    {
      name: '删除用户',
      url: '/deletePerson',
      icon: 'icon-cursor'
    },
    {
      divider: true
    }
  ]
}
