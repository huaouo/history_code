<template>
  <b-card no-body>
    <b-card-header>
      <strong>删除用户</strong>
    </b-card-header>
    <b-card-body>
      请输入要删除的用户ID:
      <b-row class="line-control" align-v="center">
        <b-col>
          <b-form-input v-model.trim="id"></b-form-input>
        </b-col>
      </b-row>
      <b-row class="line-control" align-v="center">
        <b-col class="text-center">
          <b-button @click="submit" variant="primary" class="px-4">提交</b-button>
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
    name: "AddPerson",
    data: function () {
      return {
        id: null
      }
    },
    methods: {
      submit: function () {
        axios({
          method: 'post',
          url: '/controller/deleteUser',
          data: this.id
        }).then(response => {
          if (response.data !== 'ILLEGAL') {
            this.$notify({
              group: 'notify',
              type: 'success',
              title: '成功',
              text: '删除用户成功!'
            })
          } else if (response.data === 'FAILURE') {
            this.$notify({
              group: 'notify',
              type: 'error',
              title: '成功',
              text: '删除失败, 请检查输入或数据库内部完整性约束!'
            })
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
  }
</script>
