<template>
  <div class="app flex-row align-items-center">
    <notifications group="notify" />
    <div class="container">
      <b-row class="justify-content-center">
        <b-col md="8">
          <b-card-group>
            <b-card no-body class="p-4">
              <b-card-body>
                <b-form>
                  <h1>用户登录</h1>
                  <b-input-group class="mb-3">
                    <b-input-group-prepend>
                      <b-input-group-text><i class="icon-user"></i></b-input-group-text>
                    </b-input-group-prepend>
                    <b-form-input v-model="id" :state="idState" type="text" class="form-control" placeholder="用户名"
                                  autocomplete="username email"/>
                    <b-form-invalid-feedback>
                      请输入用户名!
                    </b-form-invalid-feedback>
                  </b-input-group>
                  <b-input-group class="mb-4">
                    <b-input-group-prepend>
                      <b-input-group-text><i class="icon-lock"></i></b-input-group-text>
                    </b-input-group-prepend>
                    <b-form-input v-model="password" :state="passwordState" type="password" class="form-control"
                                  placeholder="密码" autocomplete="current-password"/>
                    <b-form-invalid-feedback>
                      密码长度必须在6-50位之间!
                    </b-form-invalid-feedback>
                  </b-input-group>
                  <b-row>
                    <b-col  class="text-center">
                      <b-button @click="submit" variant="primary" class="px-4">登录</b-button>
                    </b-col>
                  </b-row>
                </b-form>
              </b-card-body>
            </b-card>
            <b-card no-body class="text-white bg-primary py-5 d-md-down-none" style="width:44%">
              <b-card-body class="text-center">
                <div>
                  <h2>学生成绩管理系统</h2>
                  <p>欢迎使用学生成绩管理系统。</p>
                  <p>杨振华<br>软件工程1603<br>201626811225</p>
                  <p></p>
                </div>
              </b-card-body>
            </b-card>
          </b-card-group>
        </b-col>
      </b-row>
    </div>
  </div>
</template>

<script>
  import * as axios from "axios";

  export default {
    name: 'Login',
    data: () => {
      return {
        id: '',
        password: ''
      }
    },
    computed: {
      idState: function () {
        return this.id.length > 0;
      },
      passwordState: function () {
        return this.password.length >= 6 && this.password.length <= 50;
      }
    },
    methods: {
      submit: function () {
        if(this.idState && this.passwordState) {
          axios({
            method: 'post',
            url: '/controller/login',
            data: {
              id: this.id,
              password: this.password
            }
          }).then(response => {
            window.loggedStatus = response.data;
            if (window.loggedStatus !== 'ILLEGAL') {
              this.$router.push('/dashboard')
            } else {
              this.$notify({
                group: 'notify',
                type: 'error',
                title: '错误',
                text: '用户名或密码错误, 请重新输入!'
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
            text: '用户名或密码不满足条件!'
          })
        }
      }
    }
  }
</script>
