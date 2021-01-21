<template>
  <b-card no-body>
    <b-card-header>
      <strong>成绩查询</strong>
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
        学年
      </b-row>
      <hr>
      <b-row class="line-control" align-v="center">
        <b-col class="text-center">
          学年绩点: {{gpa.toFixed(3)}}
        </b-col>
      </b-row>
      <hr>
      <div v-if="classInfo !== null && classInfo.length !== 0">
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
          <b-col class="text-center">
            学期
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
          <b-col class="text-center">
            {{item.term}}
          </b-col>
        </b-row>
      </div>
      <div v-else-if="classInfo === null || classInfo.length === 0">
        <b-row class="line-control" align-v="center">
          <b-col class="text-center">
            <b-alert show>该学期无成绩!</b-alert>
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
    name: "ScoreScore",
    data: function () {
      return {
        classInfo: null,
        optionMap: null,
        academicYear: null
      }
    },
    watch: {
      academicYear: function (newVal, oldVal) {
        if (this.academicYear !== null) {
          axios({
            method: 'post',
            url: '/controller/getScoreByStudent',
            data: {
              academicYear: this.academicYear,
              term: null
            }
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
      gpa: function () {
        if (this.classInfo === null || this.classInfo.length === 0) {
          return 0;
        } else {
          let credit_sum = 0;
          let gpa_sum = 0;
          for (let item of this.classInfo) {
              credit_sum += item.credit;
              gpa_sum += (0.1*item.score-5)*item.credit;
          }
          return gpa_sum/credit_sum;
        }
      },
      academicYearOptions: function () {
        if (this.optionMap === null) {
          return [{value: null, text: '请选择'}];
        } else {
          return this.toOptions(Array.from(this.optionMap.keys()));
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
