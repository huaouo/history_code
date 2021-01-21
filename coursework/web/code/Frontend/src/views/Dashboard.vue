<template>
  <b-card no-body>
    <b-card-header>
      <strong>欢迎</strong>
    </b-card-header>
    <b-card-body>
      <b-row class="line-control"></b-row>
      <b-row class="line-control"></b-row>
      <b-alert show>欢迎使用学生成绩管理系统, 请使用左侧导航栏获取更多功能！</b-alert>
      <b-row class="line-control"></b-row>
      <b-row class="line-control"></b-row>
    </b-card-body>
  </b-card>
</template>

<style scoped>
  .line-control {
    margin-top: 1em;
    margin-bottom: 1em;
  }
</style>

<script>
  export default {
    name: 'Dashboard'
  }
</script>
