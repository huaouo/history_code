<template>
  <b-card no-body>
    <b-card-header>
      <strong>学生地区查询</strong>
    </b-card-header>
    <b-card-body>
      <b-row class="line-control" align-v="center">
        <b-col cols="1"></b-col>
        请输入学生所在地(%表示通配符):
        <b-col>
          <b-form-input v-model.trim="province"></b-form-input>
        </b-col>
        省
        <b-col>
          <b-form-input v-model.trim="city"></b-form-input>
        </b-col>
        市
        <b-col>
          <b-button @click="submit" variant="primary" class="px-4">查询</b-button>
        </b-col>
      </b-row>
      <b-alert show class="text-center">该地学生人数: {{num}}</b-alert>
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
    name: "StudentMap",
    data: function () {
      return {
        province: '',
        city: '',
        num: 0
      }
    },
    methods: {
      submit: function () {
        if (this.province !== '' && this.city !== '') {
          axios({
            method: 'post',
            url: '/controller/getStudentNumber',
            data: {
              province: this.province,
              city: this.city
            }
          }).then(response => {
            if (response.data !== 'ILLEGAL') {
              this.num = response.data
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
        } else {
          this.$notify({
            group: 'notify',
            type: 'error',
            title: '错误',
            text: '请先填写省和市!'
          })
        }
      }
    }
  }
</script>
