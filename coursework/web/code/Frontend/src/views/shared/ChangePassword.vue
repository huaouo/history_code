<template>
  <div class="animated fadeIn">
    <b-card no-body>
      <b-card-header>
        <strong>修改密码</strong>
      </b-card-header>
      <b-card-body>
        <b-row align-h="center">
          <b-col cols="6">
            <b-form-group
              label="请输入原密码:"
              label-for="previousPassword"
              :invalid-feedback="previousPasswordInvalidFeedback"
              :valid-feedback="previousPasswordValidFeedback"
              :state="previousPasswordState"
            >
              <b-form-input id="previousPassword" :state="previousPasswordState"
                            v-model="previousPassword"></b-form-input>
            </b-form-group>
          </b-col>
        </b-row>

        <b-row align-h="center">
          <b-col cols="6">
            <b-form-group
              label="请输入新密码:"
              label-for="newPassword"
              :invalid-feedback="newPasswordInvalidFeedback"
              :valid-feedback="newPasswordValidFeedback"
              :state="newPasswordState"
            >
              <b-form-input id="newPassword" :state="newPasswordState" v-model="newPassword"></b-form-input>
            </b-form-group>
          </b-col>
        </b-row>

        <b-row align-h="center">
          <b-col cols="6">
            <b-form-group
              label="请重复新密码:"
              label-for="checkPassword"
              :invalid-feedback="checkPasswordInvalidFeedback"
              :valid-feedback="checkPasswordValidFeedback"
              :state="checkPasswordState"
            >
              <b-form-input id="checkPassword" :state="checkPasswordState" v-model="checkPassword"></b-form-input>
            </b-form-group>
          </b-col>
        </b-row>

        <b-row align-h="center">
          <b-col class="text-center">
            <b-button @click="submit" variant="primary" class="px-4">提交</b-button>
          </b-col>
        </b-row>
      </b-card-body>
    </b-card>
  </div>
</template>

<script>
  import * as axios from "axios";

  export default {
    name: 'ChangePassword',
    methods: {
      submit: function () {
        axios({
          method: 'post',
          url: '/controller/changePassword',
          data: {
            previousPassword: this.previousPassword,
            newPassword: this.newPassword
          }
        }).then(response => {
          window.loggedStatus = response.data;
          if (window.loggedStatus !== 'ILLEGAL') {
            this.$notify({
              group: 'notify',
              type: 'success',
              title: '成功',
              text: '修改密码成功!'
            })
          } else {
            this.$notify({
              group: 'notify',
              type: 'error',
              title: '错误',
              text: '修改密码失败, 请检查输入!'
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
    },
    data: function () {
      return {
        previousPassword: '',
        newPassword: '',
        checkPassword: '',
        previousPasswordValidFeedback: '验证通过!',
        newPasswordValidFeedback: '验证通过!',
        checkPasswordValidFeedback: '验证通过!',
        previousPasswordInvalidFeedback: '密码长度必须在6-50位之间!',
        newPasswordInvalidFeedback: '密码长度必须在6-50位之间!',
      }
    },
    computed: {
      previousPasswordState: function () {
        return this.previousPassword.length >= 6 && this.previousPassword.length <= 50;
      },
      newPasswordState: function () {
        return this.newPassword.length >= 6 && this.newPassword.length <= 50;
      },
      checkPasswordState: function () {
        return this.newPassword === this.checkPassword && this.newPasswordState;
      },
      checkPasswordInvalidFeedback: function () {
        if (!this.newPasswordState) {
          return "请先输入正确的密码!"
        } else {
          return "密码输入不一致!"
        }
      }
    }
  }
</script>
