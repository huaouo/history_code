<template>
  <div class="animated fadeIn">
    <b-card no-body>
      <b-card-header>
        <strong>信息管理</strong>
      </b-card-header>
      <b-card-body>
        <b-row class="line-control">
          <b-col>教师编号: {{teacherId}}</b-col>
          <b-col>姓名: {{teacherName}}</b-col>
        </b-row>
        <b-row class="line-control">
          <b-col>性别: {{sex}}</b-col>
          <b-col>年龄: {{age}}</b-col>
        </b-row>
        <b-row class="line-control">
          <b-col>职称: {{title}}</b-col>
          <b-col>联系电话: {{phone}}</b-col>
        </b-row>
        <b-button @click="goChangePassword" variant="primary">修改密码</b-button>
      </b-card-body>
    </b-card>
  </div>
</template>

<style scoped>
  .line-control {
    margin-top: 2em;
    margin-bottom: 2em;
  }
</style>
<script>
  import * as axios from "axios";

  export default {
    name: 'StudentInfo',
    methods: {
      goChangePassword: function () {
        this.$router.push('/changePassword')
      }
    },
    data: function () {
      return {
        teacherId: null,
        teacherName: null,
        credit: null,
        sex: null,
        age: null,
        title: null,
        phone: null
      }
    },
    mounted: function () {
      axios({
        method: 'get',
        url: '/controller/getTeacherInfo'
      }).then(response => {
        if (response.data !== 'ILLEGAL') {
          for (let i in response.data) {
            if (response.data.hasOwnProperty(i)) {
              this[i] = response.data[i];
            }
          }
        } else {
          this.$notify({
            group: 'notify',
            type: 'error',
            title: '错误',
            text: '非法操作, 请重新登录后再试!'
          })
        }
      }).catch(error => {
        console.log(error)
        this.$notify({
          group: 'notify',
          type: 'error',
          title: '错误',
          text: '网络错误!'
        })
      })
    }
  }
</script>
