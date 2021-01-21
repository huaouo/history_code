<template>
  <div class="app">
    <notifications group="notify"/>
    <AppHeader fixed>
      <SidebarToggler class="d-lg-none" display="md" mobile/>
      <b-link class="navbar-brand" to="#">
        <span class="navbar-brand-full">学生成绩管理系统</span>
        <img class="navbar-brand-minimized" src="img/brand/sygnet.svg" width="30" height="30">
      </b-link>
      <SidebarToggler class="d-md-down-none" display="lg"/>
      <b-navbar-nav class="ml-auto">
        <DefaultHeaderDropdownAccnt/>
      </b-navbar-nav>
    </AppHeader>
    <div class="app-body">
      <AppSidebar fixed>
        <SidebarHeader/>
        <SidebarForm/>
        <SidebarNav :navItems="nav"></SidebarNav>
        <SidebarFooter/>
        <SidebarMinimizer/>
      </AppSidebar>
      <main class="main">
        <Breadcrumb :list="list"/>
        <div class="container-fluid">
          <router-view></router-view>
        </div>
      </main>
    </div>
  </div>
</template>

<script>
  import nav from '@/nav/_nav'
  import admin_nav from '@/nav/admin_nav'
  import teacher_nav from '@/nav/teacher_nav'
  import student_nav from '@/nav/student_nav'
  import {
    Header as AppHeader,
    SidebarToggler,
    Sidebar as AppSidebar,
    SidebarFooter,
    SidebarForm,
    SidebarHeader,
    SidebarMinimizer,
    SidebarNav,
    Aside as AppAside,
    AsideToggler,
    Footer as TheFooter,
    Breadcrumb
  } from '@coreui/vue'
  import DefaultHeaderDropdownAccnt from './DefaultHeaderDropdownAccnt'

  export default {
    name: 'full',
    components: {
      AsideToggler,
      AppHeader,
      AppSidebar,
      AppAside,
      TheFooter,
      Breadcrumb,
      DefaultHeaderDropdownAccnt,
      SidebarForm,
      SidebarFooter,
      SidebarToggler,
      SidebarHeader,
      SidebarNav,
      SidebarMinimizer
    },
    computed: {
      nav() {
        switch (window.loggedStatus) {
          case 'ADMIN':
            return admin_nav.items
          case 'TEACHER':
            return teacher_nav.items
          case 'STUDENT':
            return student_nav.items
        }
      },
      name() {
        return this.$route.name
      },
      list() {
        return this.$route.matched.filter((route) => route.name || route.meta.label)
      }
    }
  }
</script>
