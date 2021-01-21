<template>
  <b-card no-body>
    <b-card-header>
      <strong>开课查询</strong>
    </b-card-header>
    <b-card-body>
      <b-row align-v="center">
        <b-col>
          请选择:
        </b-col>
      </b-row>
      <b-row class="line-control" align-v="center">
        <b-col cols="3">
          <b-form-select v-model="academicYear" :options="academicYearOptions"></b-form-select>
        </b-col>
        学年 第
        <b-col cols="2">
          <b-form-select v-model="term" :options="termOptions"></b-form-select>
        </b-col>
        学期
      </b-row>
      <hr>
      <div v-if="courseAndClassMap !== null && courseAndClassMap.length !== 0">
        <b-row class="line-control" align-v="center">
          <b-col class="text-center">
            课程名称
          </b-col>
          <b-col class="text-center">
            开课专业
          </b-col>
          <b-col class="text-center">
            开课班级
          </b-col>
        </b-row>
        <b-row class="line-control" align-v="center" v-for="course in courseAndClassMap">
          <b-col class="text-center">
            {{course.courseName}}
          </b-col>
          <b-col class="text-center">
            {{course.majorName}}
          </b-col>
          <b-col class="text-center">
            {{course.classNumber}}
          </b-col>
        </b-row>
      </div>
      <div v-else-if="courseAndClassMap === null || courseAndClassMap.length === 0">
        <b-row class="line-control" align-v="center">
          <b-col class="text-center">
            <b-alert show>该学期您无开课!</b-alert>
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
    name: "TeacherCourse",
    data: function () {
      return {
        classInfo: null,
        optionMap: null,
        courseAndClassMap: null,
        academicYear: null,
        term: null,
      }
    },
    watch: {
      term: function (newVal, oldVal) {
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
              this.courseAndClassMap = response.data
              console.log(response.data)
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
      }
    },
    computed: {
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
