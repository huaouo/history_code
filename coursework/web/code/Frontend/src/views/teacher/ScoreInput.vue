<template>
  <b-card no-body>
    <b-card-header>
      <strong>成绩录入</strong>
    </b-card-header>
    <b-card-body>
      <b-row align-v="center">
        <b-col>
          请选择:
        </b-col>
      </b-row>
      <b-row class="line-control" align-v="center">
        <b-col cols="2">
          <b-form-select v-model="academicYear" :options="academicYearOptions"></b-form-select>
        </b-col>
        学年 第
        <b-col cols="auto">
          <b-form-select v-model="term" :options="termOptions"></b-form-select>
        </b-col>
        学期
        <b-col cols="3">
          <b-form-select v-model="course" :options="courseOptions"></b-form-select>
        </b-col>
        课程
        <b-col cols="3">
          <b-form-select v-model="classFullName" :options="classOptions"></b-form-select>
        </b-col>
        班
      </b-row>
      <hr>
      <div v-if="classInfo">
        <b-row class="line-control" align-v="center">
          <b-col class="text-center">
            学号
          </b-col>
          <b-col class="text-center">
            姓名
          </b-col>
          <b-col class="text-center">
            成绩
          </b-col>
        </b-row>
        <b-row class="line-control" align-v="center" v-for="(student, index) in classInfo">
          <b-col class="text-center">
            {{student.studentId}}
          </b-col>
          <b-col class="text-center">
            {{student.studentName}}
          </b-col>
          <b-col class="text-center">
            <b-form-input v-model.number="classInfo[index].score"></b-form-input>
          </b-col>
        </b-row>
        <b-row class="line-control" align-v="center">
          <b-col class="text-center">
            <b-button @click="submit" variant="primary" class="px-4">提交</b-button>
          </b-col>
        </b-row>
      </div>
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
    name: "ScoreInput",
    data: function () {
      return {
        classInfo: null,
        optionMap: null,
        courseAndClassMap: null,
        academicYear: null,
        term: null,
        course: null,
        classFullName: null
      }
    },
    watch: {
      term: function (newVal, oldVal) {
        this.course = null;
        this.classFullName = null;
        if (this.academicYear !== null && this.term !== null) {
          axios({
            method: 'post',
            url: '/controller/getTeachCourseAndClassListByTerm',
            data: {
              academicYear: this.academicYear,
              term: this.term
            }
          }).then(response => {
            if (response.data !== 'ILLEGAL') {
              let m = new Map()
              for (let i of response.data) {
                if (m.get(i.courseName) === undefined) {
                  m.set(i.courseName, [])
                }
                m.get(i.courseName).push([i.majorName, i.classNumber])
              }
              this.courseAndClassMap = m
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
      },
      classFullName: function (newVal, oldVal) {
        if (this.academicYear !== null && this.term !== null
          && this.course !== null && this.classFullName !== null) {
          axios({
            method: 'post',
            url: '/controller/getScoreDataTeacherView',
            data: {
              courseName: this.course,
              majorName: this.classFullName[0],
              classNumber: this.classFullName[1]
            }
          }).then(response => {
            if (response.data !== 'ILLEGAL') {
              this.classInfo = response.data;
              for (let i of this.classInfo) {
                if (i.score === -1) {
                  i.score = ''
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
    },
    methods: {
      toOptions: function (optionList) {
        let ret = [{value: null, text: '请选择'}];
        for (let i of optionList) {
          ret.push({value: i, text: i});
        }
        return ret;
      },
      store: function () {
        let pushData = []
        for (let i of this.classInfo) {
          if (i.score !== '') {
            pushData.push(i)
          }
        }
        axios({
          method: 'post',
          url: '/controller/storeScores',
          data: pushData
        });
      },
      submit: function () {
        this.store()
        axios({
          method: 'post',
          url: '/controller/submitScore',
          data: this.classInfo[0]
        }).then(response => {
          if (response.data !== 'ILLEGAL') {
            this.$notify({
              group: 'notify',
              type: 'success',
              title: '成功',
              text: '提交成功!'
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
    },
    computed: {
      courseOptions: function () {
        if (this.courseAndClassMap === null) {
          return [{value: null, text: '请选择'}];
        } else {
          return this.toOptions(Array.from(this.courseAndClassMap.keys()));
        }
      },
      classOptions: function () {
        if (this.courseAndClassMap === null || this.course === null) {
          return [{value: null, text: '请选择'}];
        } else {
          return this.toOptions(this.courseAndClassMap.get(this.course))
        }
      },
      academicYearOptions: function () {
        if (this.optionMap === null) {
          return [{value: null, text: '请选择'}];
        } else {
          return this.toOptions(Array.from(this.optionMap.keys()));
        }
      },
      termOptions: function () {
        if (this.optionMap === null || this.academicYear === null) {
          return [{value: null, text: '请选择'}];
        } else {
          return this.toOptions(this.optionMap.get(this.academicYear))
        }
      }
    },
    mounted: function () {
      axios({
        method: 'get',
        url: '/controller/getTermList'
      }).then(response => {
        if (response.data !== 'ILLEGAL') {
          let m = new Map()
          for (let i of response.data) {
            if (m.get(i.academicYear) === undefined) {
              m.set(i.academicYear, [])
            }
            m.get(i.academicYear).push(i.term)
          }
          this.optionMap = m
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
