<template>
  <b-card no-body>
    <b-card-header>
      <strong>增加用户</strong>
    </b-card-header>
    <b-card-body>
      请选择用户类类别:
      <b-row class="line-control" align-v="center">
        <b-col>
          <b-form-select v-model="userClass" :options="userClassOptions"></b-form-select>
        </b-col>
      </b-row>
      <hr>
      <div class="line-control">请输入信息:</div>
      <div v-if="userClass === '学生'">
        <v-row class="line-control" align-v="center">
          <v-col>
            学号:
            <b-form-input v-model.trim="id"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            姓名:
            <b-form-input v-model.trim="name"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            性别:
            <b-form-input v-model.trim="sex"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            年龄:
            <b-form-input v-model.number="age"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            班级:
            <b-form-input v-model.number="classNumber"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            专业:
            <b-form-input v-model.trim="majorName"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            生源地(填地级市, 含"市"等字样):
            <b-form-input v-model.trim="city"></b-form-input>
          </v-col>
        </v-row>
      </div>
      <div v-else-if="userClass === '教师'">
        <v-row class="line-control" align-v="center">
          <v-col>
            教师编号:
            <b-form-input v-model.trim="id"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            姓名:
            <b-form-input v-model.trim="name"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            性别:
            <b-form-input v-model.trim="sex"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            年龄:
            <b-form-input v-model.number="age"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            职称:
            <b-form-input v-model.trim="title"></b-form-input>
          </v-col>
        </v-row>
        <v-row class="line-control" align-v="center">
          <v-col>
            电话:
            <b-form-input v-model.trim="phone"></b-form-input>
          </v-col>
        </v-row>
      </div>
      <div v-else="userClass === '管理员'">
        <v-row class="line-control" align-v="center">
          <v-col>
            用户名:
            <b-form-input v-model.trim="id"></b-form-input>
          </v-col>
        </v-row>
      </div>
      <hr>
      <b-row class="line-control" align-v="center">
        <b-col>
          <small>注: 默认密码为 123456 请告知用户及时修改</small>
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
        userClass: '学生',
        userClassOptions: [
          {value: '学生', text: '学生'},
          {value: '教师', text: '教师'},
          {value: '管理员', text: '管理员'}
        ],
        id: null,
        name: null,
        sex: null,
        age: null,
        classNumber: null,
        majorName: null,
        city: null,
        title: null,
        phone: null
      }
    },
    methods: {
      submit: function () {
        let target = ''
        let data = null
        if (this.userClass === '学生') {
          data = {
            studentId: this.id,
            studentName: this.name,
            sex: this.sex,
            age: this.age,
            classNumber: this.classNumber,
            majorName: this.majorName,
            city: this.city
          }
          target = '/controller/addStudent'
        } else if (this.userClass === '教师') {
          data = {
            teacherId: this.id,
            teacherName: this.name,
            sex: this.sex,
            age: this.age,
            title: this.title,
            phone: this.phone
          }
          target = '/controller/addTeacher'
        } else {
          data = {
            id: this.id
          }
          target = '/controller/addAdmin'
        }

        axios({
          method: 'post',
          url: target,
          data: data
        }).then(response => {
          if (response.data !== 'ILLEGAL') {
            this.$notify({
              group: 'notify',
              type: 'success',
              title: '成功',
              text: '添加用户成功!'
            })
          } else if (response.data === 'FAILURE') {
            this.$notify({
              group: 'notify',
              type: 'error',
              title: '成功',
              text: '添加失败, 请检查输入!'
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
