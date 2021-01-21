<template>
  <AppHeaderDropdown right no-caret>
    <template slot="header">
      <img
        src="img/avatars/6.jpg"
        class="img-avatar"/>
    </template>
    <template slot="dropdown">
      <b-dropdown-item @click="logout"><i class="fa fa-lock"/>退出登录</b-dropdown-item>
    </template>
  </AppHeaderDropdown>
</template>

<script>
  import {HeaderDropdown as AppHeaderDropdown} from '@coreui/vue'

  export default {
    name: 'DefaultHeaderDropdownAccnt',
    components: {
      AppHeaderDropdown
    },
    methods: {
      logout: function () {
        window.loggedStatus = undefined
        this.$router.push('/pages/login')
      }
    }
  }
</script>
