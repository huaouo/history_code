<template>
  <b-card no-body>
    <b-card-header>
      <strong>学业查询</strong>
    </b-card-header>
    <b-card-body>
      <b-row align-v="center">
        <b-col class="text-center">
          学业状况:
        </b-col>
        <b-col class="text-center">
          已获得学分: {{credit}}
        </b-col>
        <b-col class="text-center">
          所有课程平均绩点: {{gpa.toFixed(3)}}
        </b-col>
      </b-row>
      <hr>
      <b-row class="line-control" align-v="center">
        <b-col class="text-center">
          课程名
        </b-col>
        <b-col class="text-center">
          成绩
        </b-col>
        <b-col class="text-center">
          学分
        </b-col>
      </b-row>
      <b-row class="line-control" align-v="center" v-for="item in classInfo">
        <b-col class="text-center">
          {{item.courseName}}
        </b-col>
        <b-col class="text-center">
          {{item.score}}
        </b-col>
        <b-col class="text-center">
          {{item.credit}}
        </b-col>
      </b-row>
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
  import * as axios from "axios";

  export default {
    name: "AcademicQuery",
    data: function () {
      return {
        classInfo: null,
        credit: null,
        gpa: null
      }
    },
    computed: {},
    mounted: function () {
      axios({
        method: 'get',
        url: '/controller/getAcademicStatus'
      }).then(response => {
        if (response.data !== 'ILLEGAL') {
          this.classInfo = response.data
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

      axios({
        method: 'get',
        url: '/controller/getStudentInfo'
      }).then(response => {
        if (response.data !== 'ILLEGAL') {
          this.credit = response.data.credit
          this.gpa = response.data.gpa
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
